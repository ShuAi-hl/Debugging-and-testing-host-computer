#ifndef CANPROTOCOLWORKER_H
#define CANPROTOCOLWORKER_H

#include "protocolbase.h"
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusFrame>
#include <QVector>

/**
 * @brief CAN 总线 Worker（基于 QtSerialBus/QCanBus）
 *
 * 支持通过 USB-CAN 适配器（如 PEAK、Vector、ZLG USBCAN 等）收发 CAN2.0B 帧。
 * QtSerialBus 提供统一接口，底层驱动由具体插件实现。
 */
class CanProtocolWorker : public ProtocolBase {
    Q_OBJECT
public:
    explicit CanProtocolWorker(QObject *parent = nullptr);
    ~CanProtocolWorker() override;

    QString protocolName() const override { return QStringLiteral("CAN"); }

    struct CanConfig {
        QString pluginName;       // 插件名："peakcan", "vectorcan", "zlgcan" 等
        QString deviceName;       // 设备名
        int bitrate = 500000;     // 波特率（bps）
    };

    /// CAN 报文 -> 数据点 解析映射
    struct CanFrameMapping {
        quint32 frameId;          // 标准帧或扩展帧 ID
        QString name;
        QString unit;
        int startByte = 0;         // 起始字节
        int length = 2;           // 数据长度
        double scale = 1.0;
        bool signed_ = false;
        bool bigEndian = true;
    };

public slots:
    void scanPorts() override;
    void openConnection() override;
    void closeConnection() override;
    void sendCommand(const QByteArray &cmd) override;

    /// 发送一帧 CAN 报文
    void sendCanFrame(quint32 id, const QByteArray &payload, bool extended = false);

    void setConfig(const CanConfig &cfg) { m_cfg = cfg; }
    const CanConfig &config() const { return m_cfg; }

    void setFrameMappings(const QVector<CanFrameMapping> &m) { m_frameMappings = m; }
    const QVector<CanFrameMapping> &frameMappings() const { return m_frameMappings; }

protected:
    bool doOpen() override;
    bool doClose() override;

private slots:
    void onFramesReceived();
    void onDeviceError(QCanBusDevice::CanBusError err);

private:
    QList<DeviceDataPoint> parseCanFrame(const QCanBusFrame &frame);
    DeviceDataPoint resolveCanFrame(const QCanBusFrame &frame, const CanFrameMapping &m) const;

    QCanBusDevice *m_device = nullptr;
    CanConfig m_cfg;
    QVector<CanFrameMapping> m_frameMappings;
};

#endif // CANPROTOCOLWORKER_H
