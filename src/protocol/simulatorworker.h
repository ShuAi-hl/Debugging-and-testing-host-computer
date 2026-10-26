#ifndef SIMULATORWORKER_H
#define SIMULATORWORKER_H

#include "protocolbase.h"
#include <QTimer>
#include <QRandomGenerator>

/**
 * @brief 数据模拟器（无硬件时替代真实协议 Worker）
 *
 * 以固定频率生成模拟的温度/压力/转速/电流数据，支持正弦波动 + 随机噪声。
 * 行为与真实协议 Worker 一致：同样被 moveToThread，同样通过信号发数据。
 */
class SimulatorWorker : public ProtocolBase {
    Q_OBJECT
public:
    explicit SimulatorWorker(QObject *parent = nullptr);
    ~SimulatorWorker() override;

    QString protocolName() const override { return QStringLiteral("Simulator"); }

    /// 模拟场景
    enum Scene { Basic, MotorRun, OvenHeat };
    void setScene(Scene s) { m_scene = s; }

public slots:
    void scanPorts() override;
    void openConnection() override;
    void closeConnection() override;
    void sendCommand(const QByteArray &cmd) override;

protected:
    bool doOpen() override;
    bool doClose() override;
    void doPoll() override;

private:
    Scene m_scene = Basic;
    qint64 m_startMs = 0;
};

#endif // SIMULATORWORKER_H
