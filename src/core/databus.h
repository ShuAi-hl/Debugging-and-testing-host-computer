#ifndef DATABUS_H
#define DATABUS_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QMutex>
#include "devicedatapoint.h"

/**
 * @brief 数据分发中心（单例）
 *
 * 所有协议 Worker 解析到数据后，通过 dataReceived 信号发布到 DataBus；
 * UI、日志、存储模块订阅 dataConsumed 即可获得最新数据。
 */
class DataBus : public QObject {
    Q_OBJECT
public:
    static DataBus &instance();

    /// 发布一条数据点（协议层调用）
    void publish(const DeviceDataPoint &dp);

    /// 批量发布
    void publishBatch(const QList<DeviceDataPoint> &list);

    /// 获取某个字段最近 N 条数据
    QList<DeviceDataPoint> recent(const QString &name, int count = 500) const;

    /// 获取所有已注册的字段名
    QStringList allKeys() const;

signals:
    /// 单条数据到达
    void dataConsumed(const DeviceDataPoint &dp);

    /// 批量数据到达
    void batchConsumed(const QList<DeviceDataPoint> &list);

private:
    explicit DataBus(QObject *parent = nullptr);
    ~DataBus() override = default;
    DataBus(const DataBus &) = delete;
    DataBus &operator=(const DataBus &) = delete;

    mutable QMutex m_mutex;
    QMap<QString, QList<DeviceDataPoint>> m_store;  // name -> history
    static constexpr int MAX_POINTS = 2000;         // 每个字段最多保留 2000 点
};

#endif // DATABUS_H
