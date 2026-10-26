#include "serialprotocolworker.h"
#include "../core/logger.h"

SerialProtocolWorker::SerialProtocolWorker(QObject *parent)
    : ProtocolBase(parent)
    , m_port(new QSerialPort(this))
{
    connect(m_port, &QSerialPort::readyRead, this, &SerialProtocolWorker::onReadyRead);
    connect(m_port, &QSerialPort::errorOccurred, this, &SerialProtocolWorker::onErrorOccurred);
}

SerialProtocolWorker::~SerialProtocolWorker()
{
    if (m_port && m_port->isOpen()) m_port->close();
}

void SerialProtocolWorker::scanPorts()
{
    // 格式: "portName|显示文本"，面板解析后把 portName 存进 itemData
    QStringList list;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const auto &info : infos) {
        QString desc = info.description();
        QString display = desc.isEmpty()
            ? info.portName()
            : QStringLiteral("%1 (%2)").arg(info.portName(), desc);
        list.append(info.portName() + QStringLiteral("|") + display);
    }
    qInfo() << "扫描串口:" << list;
    emit portsScanned(list);
}

bool SerialProtocolWorker::doOpen()
{
    if (m_cfg.portName.isEmpty()) {
        emit errorOccurred(QStringLiteral("未选择串口"));
        return false;
    }

    m_port->setPortName(m_cfg.portName);
    m_port->setBaudRate(m_cfg.baudRate);
    m_port->setDataBits(m_cfg.dataBits);
    m_port->setParity(m_cfg.parity);
    m_port->setStopBits(m_cfg.stopBits);
    m_port->setFlowControl(m_cfg.flowControl);

    if (!m_port->open(QIODevice::ReadOnly | QIODevice::WriteOnly)) {
        QString err = m_port->errorString();
        emit errorOccurred(err);
        qWarning() << "串口打开失败:" << err;
        return false;
    }

    m_rxBuffer.clear();
    return true;
}

bool SerialProtocolWorker::doClose()
{
    if (m_port && m_port->isOpen()) {
        m_port->close();
    }
    return true;
}

void SerialProtocolWorker::openConnection()
{
    if (doOpen()) {
        setConnected(true);
    }
}

void SerialProtocolWorker::closeConnection()
{
    stopPolling();
    if (doClose()) {
        setConnected(false);
    }
}

void SerialProtocolWorker::sendCommand(const QByteArray &cmd)
{
    if (!m_port || !m_port->isOpen()) {
        emit errorOccurred(QStringLiteral("串口未打开，无法发送命令"));
        return;
    }
    m_port->write(cmd);
    m_port->flush();
    qDebug() << "串口 TX:" << cmd.toHex(' ');
}

void SerialProtocolWorker::onReadyRead()
{
    QByteArray data = m_port->readAll();
    emit rawBytesReceived(data);
    qDebug() << "串口 RX:" << data.toHex(' ');

    m_rxBuffer.append(data);

    auto points = parseBuffer();
    if (!points.isEmpty()) {
        for (const auto &dp : points) {
            emit dataParsed(dp);
        }
        emit batchDataParsed(points);
    }
}

void SerialProtocolWorker::onErrorOccurred(QSerialPort::SerialPortError err)
{
    // ResourceError 在关闭时触发，忽略
    if (err == QSerialPort::NoError || err == QSerialPort::ResourceError) return;
    QString msg = m_port->errorString();
    qWarning() << "串口错误:" << msg;
    emit errorOccurred(msg);
    if (err == QSerialPort::DeviceNotFoundError ||
        err == QSerialPort::PermissionError) {
        setConnected(false);
    }
}

/**
 * @brief 解析缓冲区中的自定义帧
 *
 * 默认帧格式: $name=value%unit#
 *   例: "$temp=25.6%C#$pressure=101.3%kPa#"
 *
 * 支持多帧连续传输和半帧/粘包处理
 */
QList<DeviceDataPoint> SerialProtocolWorker::parseBuffer()
{
    QList<DeviceDataPoint> result;

    while (true) {
        int startIdx = m_rxBuffer.indexOf('$');
        if (startIdx < 0) {
            // 没有帧头，丢弃所有数据
            m_rxBuffer.clear();
            break;
        }
        if (startIdx > 0) {
            // 丢弃帧头前的噪声
            m_rxBuffer.remove(0, startIdx);
        }

        int endIdx = m_rxBuffer.indexOf('#');
        if (endIdx < 0) {
            // 没有帧尾，等待更多数据
            if (m_rxBuffer.size() > 1024) {
                // 防止缓冲区溢出
                m_rxBuffer.clear();
            }
            break;
        }

        // 提取一帧内容（不含帧头帧尾）
        QByteArray frame = m_rxBuffer.mid(1, endIdx - 1);
        m_rxBuffer.remove(0, endIdx + 1);

        // name=value%unit
        int eqPos = frame.indexOf('=');
        if (eqPos < 0) continue;
        int pctPos = frame.indexOf('%', eqPos);
        QString name = QString::fromUtf8(frame.left(eqPos)).trimmed();
        QString valStr, unit;
        if (pctPos > eqPos) {
            valStr = QString::fromUtf8(frame.mid(eqPos + 1, pctPos - eqPos - 1)).trimmed();
            unit   = QString::fromUtf8(frame.mid(pctPos + 1)).trimmed();
        } else {
            valStr = QString::fromUtf8(frame.mid(eqPos + 1)).trimmed();
        }

        bool ok = false;
        double value = valStr.toDouble(&ok);
        if (!ok) {
            qWarning() << "数值解析失败:" << valStr;
            continue;
        }

        DeviceDataPoint dp(name, value, unit, protocolName());
        result.append(dp);
    }

    return result;
}
