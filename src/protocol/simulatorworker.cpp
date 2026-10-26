#include "simulatorworker.h"
#include "../core/logger.h"
#include <QDateTime>

SimulatorWorker::SimulatorWorker(QObject *parent)
    : ProtocolBase(parent)
{
    m_pollIntervalMs = 500;  // 模拟器默认 2Hz 采样
}

SimulatorWorker::~SimulatorWorker() = default;

void SimulatorWorker::scanPorts()
{
    emit portsScanned({ QStringLiteral("(模拟器无需端口)") });
}

bool SimulatorWorker::doOpen()
{
    m_startMs = QDateTime::currentMSecsSinceEpoch();
    return true;
}

bool SimulatorWorker::doClose()
{
    return true;
}

void SimulatorWorker::openConnection()
{
    if (doOpen()) {
        setConnected(true);
        startPolling();
    }
}

void SimulatorWorker::closeConnection()
{
    stopPolling();
    if (doClose()) setConnected(false);
}

void SimulatorWorker::sendCommand(const QByteArray &cmd)
{
    Q_UNUSED(cmd);
}

static double randNoise(double lo, double hi)
{
    double r = QRandomGenerator::global()->generateDouble();  // [0,1)
    return lo + r * (hi - lo);
}

/**
 * @brief 模拟数据生成
 *
 * 三种场景：
 *   Basic     - 普通工业传感器，正弦波动
 *   MotorRun   - 电机启动/运行/停止曲线
 *   OvenHeat   - 加热炉升温 + 恒温 + 降温
 */
void SimulatorWorker::doPoll()
{
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startMs;
    double t = elapsed / 1000.0;  // 秒

    QList<DeviceDataPoint> points;

    switch (m_scene) {
    case Basic: {
        double temp = 25.0 + 3.0 * qSin(t / 5.0) + randNoise(-0.2, 0.2);
        double press = 101.3 + 0.5 * qSin(t / 8.0) + randNoise(-0.1, 0.1);
        double rpm = 1020.0 + 20.0 * qSin(t / 3.0) + randNoise(-2.0, 2.0);
        double curr = 10.0 + 0.3 * qSin(t / 4.0) + randNoise(-0.05, 0.05);

        points.append(DeviceDataPoint(QStringLiteral("温度"), temp, QStringLiteral("°C"), protocolName()));
        points.append(DeviceDataPoint(QStringLiteral("压力"), press, QStringLiteral("kPa"), protocolName()));
        points.append(DeviceDataPoint(QStringLiteral("转速"), rpm, QStringLiteral("rpm"), protocolName()));
        points.append(DeviceDataPoint(QStringLiteral("电流"), curr, QStringLiteral("A"), protocolName()));
        break;
    }
    case MotorRun: {
        double rpm;
        if (t < 5)       rpm = 1200.0 * (t / 5.0);
        else if (t < 15) rpm = 1200.0 + randNoise(-5.0, 5.0);
        else             rpm = qMax(0.0, 1200.0 * (1.0 - (t - 15) / 10.0));

        double curr = 8.0 + (rpm > 0 ? 2.0 : 0.0) + randNoise(-0.3, 0.3);
        points.append(DeviceDataPoint(QStringLiteral("转速"), rpm, QStringLiteral("rpm"), protocolName()));
        points.append(DeviceDataPoint(QStringLiteral("电流"), curr, QStringLiteral("A"), protocolName()));
        break;
    }
    case OvenHeat: {
        double temp;
        if (t < 180)      temp = 25.0 + (200.0 - 25.0) * (t / 180.0);
        else if (t < 480) temp = 200.0 + randNoise(-1.0, 1.0);
        else              temp = qMax(25.0, 200.0 - (t - 480) * 0.5);
        points.append(DeviceDataPoint(QStringLiteral("炉温"), temp, QStringLiteral("°C"), protocolName()));
        break;
    }
    }

    for (const auto &dp : points) {
        emit dataParsed(dp);
    }
    emit batchDataParsed(points);
}
