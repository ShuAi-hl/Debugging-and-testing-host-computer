#include "modbusrtuprotocolworker.h"
#include "../core/logger.h"
#include <QSerialPortInfo>

ModbusRtuProtocolWorker::ModbusRtuProtocolWorker(QObject *parent)
    : ProtocolBase(parent)
    , m_port(new QSerialPort(this))
    , m_responseTimer(new QTimer(this))
{
    m_responseTimer->setSingleShot(true);
    connect(m_port, &QSerialPort::readyRead, this, &ModbusRtuProtocolWorker::onReadyRead);
    connect(m_port, &QSerialPort::errorOccurred, this, &ModbusRtuProtocolWorker::onErrorOccurred);
    connect(m_responseTimer, &QTimer::timeout, this, &ModbusRtuProtocolWorker::onResponseTimeout);

    // 默认寄存器映射：假设从 0x0000 开始 4 个寄存器分别是 温度/压力/转速/电流
    m_mappings = {
        {0x0000, QStringLiteral("温度"), QStringLiteral("°C"),   0.1, false},
        {0x0001, QStringLiteral("压力"), QStringLiteral("kPa"),  0.01, false},
        {0x0002, QStringLiteral("转速"), QStringLiteral("rpm"),  1.0,  false},
        {0x0003, QStringLiteral("电流"), QStringLiteral("A"),    0.01, false},
    };
}

ModbusRtuProtocolWorker::~ModbusRtuProtocolWorker()
{
    if (m_port && m_port->isOpen()) m_port->close();
}

quint16 ModbusRtuProtocolWorker::computeCrc16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (unsigned char byte : data) {
        crc ^= (quint16)byte;
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void ModbusRtuProtocolWorker::scanPorts()
{
    // 格式: "portName|显示文本"
    QStringList list;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto &info : infos) {
        QString desc = info.description();
        QString display = desc.isEmpty()
            ? info.portName()
            : QStringLiteral("%1 (%2)").arg(info.portName(), desc);
        list.append(info.portName() + QStringLiteral("|") + display);
    }
    emit portsScanned(list);
}

bool ModbusRtuProtocolWorker::doOpen()
{
    if (m_cfg.portName.isEmpty()) {
        emit errorOccurred(QStringLiteral("未选择串口"));
        return false;
    }

    m_port->setPortName(m_cfg.portName);
    m_port->setBaudRate(m_cfg.baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(m_cfg.parity);
    m_port->setStopBits(m_cfg.stopBits);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port->open(QIODevice::ReadOnly | QIODevice::WriteOnly)) {
        QString err = m_port->errorString();
        emit errorOccurred(err);
        return false;
    }

    m_rxBuffer.clear();
    m_waitingResponse = false;
    m_mappingIdx = 0;
    return true;
}

bool ModbusRtuProtocolWorker::doClose()
{
    m_responseTimer->stop();
    if (m_port && m_port->isOpen()) {
        m_port->close();
    }
    return true;
}

void ModbusRtuProtocolWorker::openConnection()
{
    if (doOpen()) setConnected(true);
}

void ModbusRtuProtocolWorker::closeConnection()
{
    stopPolling();
    if (doClose()) setConnected(false);
}

void ModbusRtuProtocolWorker::sendCommand(const QByteArray &cmd)
{
    // 原始 Modbus 帧直接发送
    if (!m_port || !m_port->isOpen()) {
        emit errorOccurred(QStringLiteral("端口未打开"));
        return;
    }
    m_port->write(cmd);
    m_port->flush();
    qDebug() << "Modbus TX:" << cmd.toHex(' ');
}

void ModbusRtuProtocolWorker::writeRegister(quint16 addr, quint16 value)
{
    // 功能码 0x06: 写单个寄存器
    QByteArray frame;
    frame.append((char)m_cfg.slaveId);
    frame.append((char)0x06);
    frame.append((char)((addr >> 8) & 0xFF));
    frame.append((char)(addr & 0xFF));
    frame.append((char)((value >> 8) & 0xFF));
    frame.append((char)(value & 0xFF));

    quint16 crc = computeCrc16(frame);
    frame.append((char)(crc & 0xFF));          // CRC L
    frame.append((char)((crc >> 8) & 0xFF));    // CRC H
    sendCommand(frame);
}

QByteArray ModbusRtuProtocolWorker::makeReadRequest(quint16 startAddr, quint16 count)
{
    QByteArray frame;
    frame.append((char)m_cfg.slaveId);
    frame.append((char)0x03);                   // 读保持寄存器
    frame.append((char)((startAddr >> 8) & 0xFF));
    frame.append((char)(startAddr & 0xFF));
    frame.append((char)((count >> 8) & 0xFF));
    frame.append((char)(count & 0xFF));

    quint16 crc = computeCrc16(frame);
    frame.append((char)(crc & 0xFF));          // CRC L
    frame.append((char)((crc >> 8) & 0xFF));    // CRC H
    return frame;
}

void ModbusRtuProtocolWorker::sendReadRequest(quint16 startAddr, quint16 count)
{
    if (!m_port || !m_port->isOpen()) return;
    if (m_waitingResponse) return;  // 上一个请求还在等响应

    QByteArray frame = makeReadRequest(startAddr, count);
    m_port->write(frame);
    m_port->flush();
    m_waitingResponse = true;
    m_pollAddr = startAddr;
    m_pollCount = count;
    m_responseTimer->start(m_cfg.responseTimeoutMs);

    qDebug() << "Modbus Read TX:" << frame.toHex(' ');
}

/**
 * @brief 轮询：按映射表顺序逐个读取寄存器
 *
 * 这里用一种简单策略：每次轮询读取映射表中所有连续寄存器（合并成一次请求）。
 * 如果寄存器不连续，则按顺序分别请求。
 */
void ModbusRtuProtocolWorker::doPoll()
{
    if (m_mappings.isEmpty()) return;
    if (m_waitingResponse) return;

    // 计算需要读取的起始地址和数量（取整个映射表的范围）
    quint16 minAddr = m_mappings.first().address;
    quint16 maxAddr = m_mappings.first().address;
    for (const auto &m : m_mappings) {
        if (m.address < minAddr) minAddr = m.address;
        if (m.address > maxAddr) maxAddr = m.address;
    }
    quint16 count = maxAddr - minAddr + 1;

    sendReadRequest(minAddr, count);
}

void ModbusRtuProtocolWorker::onReadyRead()
{
    QByteArray data = m_port->readAll();
    emit rawBytesReceived(data);
    qDebug() << "Modbus RX:" << data.toHex(' ');

    m_rxBuffer.append(data);

    // 最小 Modbus RTU 响应长度：从站ID + 功能码 + 字节数 + N字节数据 + CRC2 = 5 + N
    while (m_rxBuffer.size() >= 5) {
        // 检查 CRC 长度至少够
        if (m_rxBuffer.size() < 5) break;

        quint8 func = (quint8)m_rxBuffer[1];
        if (func == 0x03 || func == 0x04) {
            // 读寄存器响应
            quint8 byteCount = (quint8)m_rxBuffer[2];
            int totalLen = 5 + byteCount;  // 1+1+1+N+2
            if (m_rxBuffer.size() < totalLen) break;

            QByteArray frame = m_rxBuffer.left(totalLen);
            m_rxBuffer.remove(0, totalLen);
            parseResponse(frame);
        } else if (func == 0x06 || func == 0x10) {
            // 写响应：8 字节固定
            if (m_rxBuffer.size() < 8) break;
            QByteArray frame = m_rxBuffer.left(8);
            m_rxBuffer.remove(0, 8);
            parseResponse(frame);
        } else {
            // 未知功能码，丢弃一个字节继续
            m_rxBuffer.remove(0, 1);
        }
    }

    // 防止堆积
    if (m_rxBuffer.size() > 256) m_rxBuffer.clear();
}

void ModbusRtuProtocolWorker::parseResponse(const QByteArray &frame)
{
    if (frame.size() < 5) return;

    // 校验 CRC
    QByteArray dataPart = frame.left(frame.size() - 2);
    quint16 recvCrc = (quint8)frame[frame.size() - 1] << 8 | (quint8)frame[frame.size() - 2];
    quint16 calcCrc = computeCrc16(dataPart);
    if (recvCrc != calcCrc) {
        qWarning() << "Modbus CRC 错误, recv=" << Qt::hex << recvCrc << "calc=" << calcCrc;
        return;
    }

    m_responseTimer->stop();
    m_waitingResponse = false;

    quint8 slave = (quint8)frame[0];
    quint8 func  = (quint8)frame[1];

    // 异常响应：功能码 | 0x80
    if (func & 0x80) {
        quint8 exceptionCode = (quint8)frame[2];
        qWarning() << "Modbus 异常响应, slave=" << slave
                   << "exception code=" << exceptionCode;
        return;
    }

    if (func == 0x03 || func == 0x04) {
        quint8 byteCount = (quint8)frame[2];
        const char *dataPtr = frame.constData() + 3;
        int registerCount = byteCount / 2;

        for (int i = 0; i < registerCount; ++i) {
            quint16 raw = ((quint8)dataPtr[i * 2] << 8) | (quint8)dataPtr[i * 2 + 1];
            quint16 addr = m_pollAddr + i;
            DeviceDataPoint dp = resolveRegister(addr, raw);
            if (!dp.name.isEmpty()) {
                emit dataParsed(dp);
            }
        }
    }

    qDebug() << "Modbus 响应解析完成";
}

DeviceDataPoint ModbusRtuProtocolWorker::resolveRegister(quint16 addr, quint16 raw) const
{
    for (const auto &m : m_mappings) {
        if (m.address == addr) {
            double value;
            if (m.signed_) {
                qint16 signedRaw = (qint16)raw;
                value = signedRaw * m.scale;
            } else {
                value = raw * m.scale;
            }
            return DeviceDataPoint(m.name, value, m.unit, protocolName());
        }
    }
    // 未在映射表中，返回默认
    return DeviceDataPoint();
}

void ModbusRtuProtocolWorker::onErrorOccurred(QSerialPort::SerialPortError err)
{
    if (err == QSerialPort::NoError || err == QSerialPort::ResourceError) return;
    QString msg = m_port->errorString();
    emit errorOccurred(msg);
    if (err == QSerialPort::DeviceNotFoundError ||
        err == QSerialPort::PermissionError) {
        setConnected(false);
    }
}

void ModbusRtuProtocolWorker::onResponseTimeout()
{
    m_waitingResponse = false;
    qWarning() << "Modbus 响应超时";
    emit errorOccurred(QStringLiteral("Modbus 响应超时"));
}
