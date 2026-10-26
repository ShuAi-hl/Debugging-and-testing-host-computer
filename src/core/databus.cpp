#include "databus.h"

DataBus &DataBus::instance()
{
    static DataBus inst;
    return inst;
}

DataBus::DataBus(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<DeviceDataPoint>("DeviceDataPoint");
}

void DataBus::publish(const DeviceDataPoint &dp)
{
    {
        QMutexLocker locker(&m_mutex);
        auto &list = m_store[dp.name];
        list.append(dp);
        if (list.size() > MAX_POINTS) {
            list.removeFirst();
        }
    }
    emit dataConsumed(dp);
}

void DataBus::publishBatch(const QList<DeviceDataPoint> &list)
{
    {
        QMutexLocker locker(&m_mutex);
        for (const auto &dp : list) {
            auto &dst = m_store[dp.name];
            dst.append(dp);
            if (dst.size() > MAX_POINTS) {
                dst.removeFirst();
            }
        }
    }
    emit batchConsumed(list);
}

QList<DeviceDataPoint> DataBus::recent(const QString &name, int count) const
{
    QMutexLocker locker(&m_mutex);
    auto it = m_store.find(name);
    if (it == m_store.end()) return {};
    const auto &list = it.value();
    if (count >= list.size()) return list;
    return list.mid(list.size() - count);
}

QStringList DataBus::allKeys() const
{
    QMutexLocker locker(&m_mutex);
    return m_store.keys();
}
