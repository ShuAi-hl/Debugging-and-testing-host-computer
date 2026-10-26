#ifndef DEVICEDATAPOINT_H
#define DEVICEDATAPOINT_H

#include <QObject>
#include <QMetaType>
#include <QString>
#include <QVariant>
#include <QDateTime>

struct DeviceDataPoint {
    Q_GADGET
public:
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(double value MEMBER value)
    Q_PROPERTY(QString unit MEMBER unit)
    Q_PROPERTY(QDateTime timestamp MEMBER timestamp)
    Q_PROPERTY(QString source MEMBER source)
    Q_PROPERTY(bool valid MEMBER valid)

    QString name;
    double value = 0.0;
    QString unit;
    QDateTime timestamp;
    QString source;      // "Serial", "Modbus", "CAN", "Simulator"
    bool valid = true;   // 是否是有效值

    DeviceDataPoint() = default;
    DeviceDataPoint(const QString &n, double v, const QString &u = QString(),
                    const QString &src = QStringLiteral("Unknown"))
        : name(n), value(v), unit(u), timestamp(QDateTime::currentDateTime()), source(src) {}

    QString toString() const {
        return QStringLiteral("[%1] %2 = %3 %4 (%5)")
            .arg(timestamp.toString(QStringLiteral("HH:mm:ss.zzz")))
            .arg(name)
            .arg(value, 0, 'f', 2)
            .arg(unit)
            .arg(source);
    }
};
Q_DECLARE_METATYPE(DeviceDataPoint)

#endif // DEVICEDATAPOINT_H
