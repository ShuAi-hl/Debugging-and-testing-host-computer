#include "modbustcpprotocolworker.h"
#include "../core/logger.h"

ModbusTcpProtocolWorker::ModbusTcpProtocolWorker(QObject *parent)
    : ProtocolBase(parent)
    , m_socket(new QTcpSocket(this))
    , m_responseTimer(new QTimer(this))
{
    m_responseTimer->setSingleShot(true);
    connect(m_socket, &QTcpSocket::readyRead, this, &ModbusTcpProtocolWorker::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ModbusTcpProtocolWorker::onSocketError);
    connect(m_responseTimer, &QTimer::timeout, this, &ModbusTcpProtocolWorker::onResponseTimeout);

    // 默认寄存器映射（与 RTU 保持一致）
    m_mappings = {
        {0x0000, QStringLiteral("温度"), QStringLiteral("°C"),   0.1,  false},
        {0x0001, QStringLiteral("压力"), QStringLiteral("kPa"),  0.01, false},
        {0x0002, QStringLiteral("转速"), QStringLiteral("rpm"),  1.0,  false},
        {0x0003, QStringLiteral("电流"), QStringLiteral("A"),    0.01, false},
    };
}

ModbusTcpProtocolWorker::~ModbusTcpProtocolWorker()
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();
}

void ModbusTcpProtocolWorker::scanPorts()
{
    // TCP 没有可扫描的"端口"概念，直接返回空
    emit portsScanned(QStringList());
}

bool ModbusTcpProtocolWorker::doOpen()
{
    if (m_cfg.host.isEmpty()) {
        emit errorOccurred(QStringLiteral("未填写目标 IP"));
        return false;
    }

    m_socket->connectToHost(m_cfg.host, m_cfg.port);
    if (!m_socket->waitForConnected(3000)) {
        emit errorOccurred(QStringLiteral("连接 %1:%2 失败: %3")
                               .arg(m_cfg.host).arg(m_cfg.port).arg(m_socket->errorString()));
        return false;
    }

    m_rxBuffer.clear();
    m_waitingResponse = false;
    m_transactionId = 0;
    qInfo() << "Modbus TCP 已连接:" << m_cfg.host << ":" << m_cfg.port;
    return true;
}

bool ModbusTcpProtocolWorker::doClose()
{
    m_responseTimer->stop();
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForDisconnected(1000);
    }
    return true;
}

void ModbusTcpProtocolWorker::openConnection()
{
    if (doOpen()) setConnected(true);
}

void ModbusTcpProtocolWorker::closeConnection()
{
    stopPolling();
    if (doClose()) setConnected(false);
}

void ModbusTcpProtocolWorker::sendCommand(const QByteArray &cmd)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        emit errorOccurred(QStringLiteral("TCP 未连接"));
        return;
    }
    m_socket->write(cmd);
    m_socket->flush();
    qDebug() << "ModbusTCP TX:" << cmd.toHex(' ');
}

void ModbusTcpProtocolWorker::writeRegister(quint16 addr, quint16 value)
{
    // MBAP(7) + PDU(5): UnitID + 0x06 + addr(2) + value(2)
    ++m_transactionId;
    QByteArray pdu;
    pdu.append((char)0x06);
    pdu.append((char)((addr >> 8) & 0xFF));
    pdu.append((char)(addr & 0xFF));
    pdu.append((char)((value >> 8) & 0xFF));
    pdu.append((char)(value & 0xFF));

    QByteArray frame;
    frame.append((char)((m_transactionId >> 8) & 0xFF));
    frame.append((char)(m_transactionId & 0xFF));
    frame.append((char)0x00);           // Protocol ID Hi
    frame.append((char)0x00);           // Protocol ID Lo
    quint16 len = 1 + pdu.size();       // UnitID + PDU
    frame.append((char)((len >> 8) & 0xFF));
    frame.append((char)(len & 0xFF));
    frame.append((char)m_cfg.unitId);
    frame.append(pdu);

    sendCommand(frame);
}

QByteArray ModbusTcpProtocolWorker::makeReadRequest(quint16 startAddr, quint16 count)
{
    ++m_transactionId;
    QByteArray pdu;
    pdu.append((char)0x03);             // 读保持寄存器
    pdu.append((char)((startAddr >> 8) & 0xFF));
    pdu.append((char)(startAddr & 0xFF));
    pdu.append((char)((count >> 8) & 0xFF));
    pdu.append((char)(count & 0xFF));

    QByteArray frame;
    frame.append((char)((m_transactionId >> 8) & 0xFF));
    frame.append((char)(m_transactionId & 0xFF));
    frame.append((char)0x00);
    frame.append((char)0x00);
    quint16 len = 1 + pdu.size();
    frame.append((char)((len >> 8) & 0xFF));
    frame.append((char)(len & 0xFF));
    frame.append((char)m_cfg.unitId);
    frame.append(pdu);
    return frame;
}

void ModbusTcpProtocolWorker::sendReadRequest(quint16 startAddr, quint16 count)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) return;
    if (m_waitingResponse) return;

    QByteArray frame = makeReadRequest(startAddr, count);
    m_socket->write(frame);
    m_socket->flush();
    m_waitingResponse = true;
    m_pollAddr = startAddr;
    m_pollCount = count;
    m_responseTimer->start(m_cfg.responseTimeoutMs);

    qDebug() << "ModbusTCP Read TX:" << frame.toHex(' ');
}

void ModbusTcpProtocolWorker::doPoll()
{
    if (m_mappings.isEmpty()) return;
    if (m_waitingResponse) return;

    quint16 minAddr = m_mappings.first().address;
    quint16 maxAddr = m_mappings.first().address;
    for (const auto &m : m_mappings) {
        if (m.address < minAddr) minAddr = m.address;
        if (m.address > maxAddr) maxAddr = m.address;
    }
    sendReadRequest(minAddr, maxAddr - minAddr + 1);
}

void ModbusTcpProtocolWorker::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit rawBytesReceived(data);
    qDebug() << "ModbusTCP RX:" << data.toHex(' ');

    m_rxBuffer.append(data);

    // MBAP 头 7 字节 + 至少 1 字节功能码
    while (m_rxBuffer.size() >= 8) {
        // Length 字段 = MBAP 第 5-6 字节（从 UnitID 开始计）
        quint16 length = ((quint8)m_rxBuffer[4] << 8) | (quint8)m_rxBuffer[5];
        int totalLen = 6 + length;  // TransactionID(2)+ProtocolID(2)+Length(2) + length字节
        if (m_rxBuffer.size() < totalLen) break;

        QByteArray frame = m_rxBuffer.left(totalLen);
        m_rxBuffer.remove(0, totalLen);
        parseResponse(frame);
    }

    if (m_rxBuffer.size() > 4096) m_rxBuffer.clear();
}

void ModbusTcpProtocolWorker::parseResponse(const QByteArray &frame)
{
    if (frame.size() < 9) return;  // MBAP(7) + FuncCode(1) + ByteCount(1)

    m_responseTimer->stop();
    m_waitingResponse = false;

    quint8 func = (quint8)frame[7];    // PDU 功能码在 MBAP 之后

    if (func & 0x80) {
        quint8 exceptionCode = (quint8)frame[8];
        qWarning() << "ModbusTCP 异常响应, code=" << exceptionCode;
        return;
    }

    if (func == 0x03 || func == 0x04) {
        quint8 byteCount = (quint8)frame[8];
        const char *dataPtr = frame.constData() + 9;
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
}

DeviceDataPoint ModbusTcpProtocolWorker::resolveRegister(quint16 addr, quint16 raw) const
{
    for (const auto &m : m_mappings) {
        if (m.address == addr) {
            double value;
            if (m.signed_) {
                value = (qint16)raw * m.scale;
            } else {
                value = raw * m.scale;
            }
            return DeviceDataPoint(m.name, value, m.unit, protocolName());
        }
    }
    return DeviceDataPoint();
}

void ModbusTcpProtocolWorker::onSocketError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err);
    emit errorOccurred(m_socket->errorString());
    if (m_socket->state() != QAbstractSocket::ConnectedState)
        setConnected(false);
}

void ModbusTcpProtocolWorker::onResponseTimeout()
{
    m_waitingResponse = false;
    qWarning() << "ModbusTCP 响应超时";
    emit errorOccurred(QStringLiteral("Modbus TCP 响应超时"));
}
