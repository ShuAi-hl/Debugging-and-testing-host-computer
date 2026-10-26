#ifndef IPROTOCOLWORKER_H
#define IPROTOCOLWORKER_H

#include <QObject>
#include <QString>
#include <QList>
#include "../core/devicedatapoint.h"

/**
 * @brief 协议 Worker 公共接口（信号/槽约定）
 *
 * 所有具体协议 Worker（Serial/Modbus/CAN）都继承自 ProtocolBase，
 * 并遵守此接口约定。Worker 通常被 moveToThread，UI 通过信号驱动其行为。
 */
class IProtocolWorker : public QObject {
    Q_OBJECT
public:
    explicit IProtocolWorker(QObject *parent = nullptr) : QObject(parent) {}
    ~IProtocolWorker() override = default;

    virtual QString protocolName() const = 0;  // "Serial", "ModbusRTU", "CAN"

public slots:
    /// 请求扫描可用端口（串口列表 / CAN 接口列表）
    virtual void scanPorts() = 0;

    /// 请求建立连接
    virtual void openConnection() = 0;

    /// 请求断开连接
    virtual void closeConnection() = 0;

    /// 请求下发命令（协议相关）
    virtual void sendCommand(const QByteArray &cmd) = 0;

signals:
    /// 发现了可用端口列表
    void portsScanned(const QStringList &ports);

    /// 连接状态变化（true = 已连接）
    void connectionStateChanged(bool connected);

    /// 连接错误信息
    void errorOccurred(const QString &message);

    /// 收到解析后的数据点（单条）
    void dataParsed(const DeviceDataPoint &dp);

    /// 收到解析后的数据点（批量）
    void batchDataParsed(const QList<DeviceDataPoint> &list);

    /// 收到原始字节流（调试用）
    void rawBytesReceived(const QByteArray &data);
};

#endif // IPROTOCOLWORKER_H
