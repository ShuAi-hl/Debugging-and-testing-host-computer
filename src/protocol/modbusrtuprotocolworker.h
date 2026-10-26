#ifndef MODBUSRTUPROTOCOLWORKER_H
#define MODBUSRTUPROTOCOLWORKER_H

#include "protocolbase.h"
#include <QSerialPort>
#include <QTimer>
#include <QVector>

/**
 * @brief 自实现的 Modbus RTU 协议 Worker
 *
 * 支持功能码 0x03（读保持寄存器）和 0x06（写单个寄存器）。
 * 通过 QSerialPort 异步收发，内置 Modbus CRC16 计算与从站响应解析。
 *
 * 寄存器映射表：
 *   address -> { name, unit, scale, signed }
 * 解析时按映射表把原始寄存器值转换成有物理意义的数据点。
 */
class ModbusRtuProtocolWorker : public ProtocolBase {
    Q_OBJECT
public:
    explicit ModbusRtuProtocolWorker(QObject *parent = nullptr);
    ~ModbusRtuProtocolWorker() override;

    QString protocolName() const override { return QStringLiteral("ModbusRTU"); }

    struct ModbusConfig {
        QString portName;
        qint32 baudRate = 9600;
        quint8 slaveId = 1;
        QSerialPort::Parity parity = QSerialPort::EvenParity;  // Modbus 常见
        QSerialPort::StopBits stopBits = QSerialPort::TwoStop;
        int responseTimeoutMs = 500;
    };

    struct RegisterMapping {
        quint16 address;
        QString name;
        QString unit;
        double scale = 1.0;      // 物理值 = raw * scale
        bool signed_ = false;    // 是否为 16 位有符号
    };

public slots:
    void scanPorts() override;
    void openConnection() override;
    void closeConnection() override;
    void sendCommand(const QByteArray &cmd) override;

    /// 直接写一个寄存器（功能码 0x06）
    void writeRegister(quint16 addr, quint16 value);

    /// 设置轮询的寄存器映射表
    void setMappings(const QVector<RegisterMapping> &m) { m_mappings = m; }
    const QVector<RegisterMapping> &mappings() const { return m_mappings; }

    /// 设置/获取配置
    void setConfig(const ModbusConfig &cfg) { m_cfg = cfg; }
    const ModbusConfig &config() const { return m_cfg; }

protected:
    bool doOpen() override;
    bool doClose() override;
    void doPoll() override;

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError err);
    void onResponseTimeout();

private:
    /// 构造 Modbus RTU 请求帧（读保持寄存器 0x03）
    QByteArray makeReadRequest(quint16 startAddr, quint16 count);

    /// 发送请求并等待响应（异步模式下用响应超时定时器）
    void sendReadRequest(quint16 startAddr, quint16 count);

    /// 解析响应帧并触发数据点
    void parseResponse(const QByteArray &frame);

    /// 解析寄存器值（根据映射表）
    DeviceDataPoint resolveRegister(quint16 addr, quint16 raw) const;

    static quint16 computeCrc16(const QByteArray &data);

    QSerialPort *m_port = nullptr;
    QByteArray   m_rxBuffer;

    ModbusConfig m_cfg;
    QVector<RegisterMapping> m_mappings;

    QTimer *m_responseTimer = nullptr;     // 响应超时定时器
    bool    m_waitingResponse = false;
    quint16 m_pollAddr = 0;                // 当前正在轮询的起始地址
    quint16 m_pollCount = 0;

    int     m_mappingIdx = 0;              // 轮询游标
};

#endif // MODBUSRTUPROTOCOLWORKER_H
