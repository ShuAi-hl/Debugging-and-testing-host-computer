#include "canprotocolworker.h"
#include "../core/logger.h"

CanProtocolWorker::CanProtocolWorker(QObject *parent)
    : ProtocolBase(parent)
{
    // 默认映射：根据 CAN ID 解析典型信号
    m_frameMappings = {
        {0x100, QStringLiteral("电机转速"), QStringLiteral("rpm"),  0, 2, 1.0,  false, true},
        {0x200, QStringLiteral("电池电压"), QStringLiteral("V"),    0, 2, 0.01, false, true},
        {0x201, QStringLiteral("电池电流"), QStringLiteral("A"),    0, 2, 0.01, true,  true},
        {0x300, QStringLiteral("冷却液温度"), QStringLiteral("°C"), 0, 1, 1.0,  false, true},
    };
}

CanProtocolWorker::~CanProtocolWorker()
{
    if (m_device) {
        m_device->disconnect();
        delete m_device;
        m_device = nullptr;
    }
}

void CanProtocolWorker::scanPorts()
{
    // QtSerialBus 插件需要已安装的驱动，这里尝试列出常见插件
    QStringList availablePlugins = QCanBus::instance()->plugins();
    qInfo() << "可用 CAN 插件:" << availablePlugins;

    // 尝试在每个插件下列出设备
    QStringList result;
    for (const auto &plugin : availablePlugins) {
        QString errMsg;
        QList<QCanBusDeviceInfo> infos = QCanBus::instance()->availableDevices(plugin, &errMsg);
        for (const auto &info : infos) {
            QString display = QStringLiteral("[%1] %2").arg(plugin, info.name());
            result.append(plugin + QStringLiteral("|") + info.name()
                          + QStringLiteral("|") + display);
        }
    }

    // 如果没有任何插件，至少返回一个占位符
    if (result.isEmpty()) {
        qWarning() << "未发现可用的 CAN 设备插件";
        result.append(QStringLiteral("(需要安装 Qt SerialBus CAN 插件)"));
    }

    emit portsScanned(result);
}

bool CanProtocolWorker::doOpen()
{
    if (m_cfg.pluginName.isEmpty() || m_cfg.deviceName.isEmpty()) {
        emit errorOccurred(QStringLiteral("请先选择 CAN 插件和设备"));
        return false;
    }

    QString errStr;
    m_device = QCanBus::instance()->createDevice(m_cfg.pluginName, m_cfg.deviceName, &errStr);
    if (!m_device) {
        emit errorOccurred(QStringLiteral("创建设备失败: %1").arg(errStr));
        return false;
    }

    // 设置波特率
    m_device->setConfigurationParameter(QCanBusDevice::BitRateKey, m_cfg.bitrate);
    // 连接原始帧过滤：接收所有帧
    m_device->setConfigurationParameter(QCanBusDevice::RawFilterKey, QByteArray());

    connect(m_device, &QCanBusDevice::framesReceived,
            this, &CanProtocolWorker::onFramesReceived);
    connect(m_device, &QCanBusDevice::errorOccurred,
            this, &CanProtocolWorker::onDeviceError);

    if (!m_device->connectDevice()) {
        QString err = m_device->errorString();
        emit errorOccurred(QStringLiteral("连接 CAN 设备失败: %1").arg(err));
        delete m_device;
        m_device = nullptr;
        return false;
    }

    qInfo() << "CAN 设备已连接:" << m_cfg.pluginName << m_cfg.deviceName
            << "@" << m_cfg.bitrate << "bps";
    return true;
}

bool CanProtocolWorker::doClose()
{
    if (m_device) {
        m_device->disconnectDevice();
        m_device->disconnect();
        delete m_device;
        m_device = nullptr;
    }
    return true;
}

void CanProtocolWorker::openConnection()
{
    if (doOpen()) setConnected(true);
}

void CanProtocolWorker::closeConnection()
{
    stopPolling();
    if (doClose()) setConnected(false);
}

void CanProtocolWorker::sendCommand(const QByteArray &cmd)
{
    // 将原始字节当作标准帧 payload 发送
    QCanBusFrame frame;
    frame.setFrameId(0x7DF);  // 通用请求 ID
    frame.setPayload(cmd);
    if (m_device) {
        m_device->writeFrame(frame);
    }
}

void CanProtocolWorker::sendCanFrame(quint32 id, const QByteArray &payload, bool extended)
{
    if (!m_device) return;
    QCanBusFrame frame;
    frame.setFrameId(id);
    frame.setPayload(payload);
    if (extended) frame.setExtendedFrameFormat(true);
    m_device->writeFrame(frame);
    qDebug() << "CAN TX id=" << Qt::hex << id << "payload=" << payload.toHex(' ');
}

void CanProtocolWorker::onFramesReceived()
{
    if (!m_device) return;

    QList<QCanBusFrame> frames = m_device->readAllFrames();
    for (const auto &frame : frames) {
        qDebug() << "CAN RX id=" << Qt::hex << frame.frameId()
                 << "payload=" << frame.payload().toHex(' ');
        emit rawBytesReceived(frame.payload());

        auto points = parseCanFrame(frame);
        for (const auto &dp : points) {
            emit dataParsed(dp);
        }
        if (!points.isEmpty()) {
            emit batchDataParsed(points);
        }
    }
}

QList<DeviceDataPoint> CanProtocolWorker::parseCanFrame(const QCanBusFrame &frame)
{
    QList<DeviceDataPoint> result;
    quint32 id = frame.frameId();

    for (const auto &m : m_frameMappings) {
        if (m.frameId == id) {
            DeviceDataPoint dp = resolveCanFrame(frame, m);
            if (!dp.name.isEmpty()) result.append(dp);
        }
    }
    return result;
}

DeviceDataPoint CanProtocolWorker::resolveCanFrame(const QCanBusFrame &frame, const CanFrameMapping &m) const
{
    const QByteArray &payload = frame.payload();
    if (m.startByte + m.length > payload.size()) return DeviceDataPoint();

    double rawValue = 0;
    if (m.length == 1) {
        rawValue = (quint8)payload[m.startByte];
        if (m.signed_ && rawValue > 127) rawValue -= 256;
    } else if (m.length == 2) {
        quint16 raw;
        if (m.bigEndian) {
            raw = ((quint8)payload[m.startByte] << 8) | (quint8)payload[m.startByte + 1];
        } else {
            raw = ((quint8)payload[m.startByte + 1] << 8) | (quint8)payload[m.startByte];
        }
        if (m.signed_ && raw > 32767) {
            rawValue = (qint16)raw;
        } else {
            rawValue = raw;
        }
    } else if (m.length == 4) {
        quint32 raw;
        if (m.bigEndian) {
            raw = ((quint8)payload[m.startByte] << 24)
                | ((quint8)payload[m.startByte + 1] << 16)
                | ((quint8)payload[m.startByte + 2] << 8)
                |  (quint8)payload[m.startByte + 3];
        } else {
            raw = ((quint8)payload[m.startByte + 3] << 24)
                | ((quint8)payload[m.startByte + 2] << 16)
                | ((quint8)payload[m.startByte + 1] << 8)
                |  (quint8)payload[m.startByte];
        }
        rawValue = (qint32)raw;
    }

    double physical = rawValue * m.scale;
    return DeviceDataPoint(m.name, physical, m.unit, protocolName());
}

void CanProtocolWorker::onDeviceError(QCanBusDevice::CanBusError err)
{
    Q_UNUSED(err);
    if (!m_device) return;
    QString msg = m_device->errorString();
    qWarning() << "CAN 设备错误:" << msg;
    emit errorOccurred(msg);
}
