#ifndef MODBUSTCPPROTOCOLWORKER_H
#define MODBUSTCPPROTOCOLWORKER_H

#include "protocolbase.h"
#include <QTcpSocket>
#include <QTimer>
#include <QVector>

/**
 * @brief 自实现的 Modbus TCP 协议 Worker
 *
 * 与 Modbus RTU 的区别：
 *   - 传输层：QTcpSocket（端口默认 502）
 *   - 帧头：MBAP（TransactionID 2 + ProtocolID 2 + Length 2 + UnitID 1）
 *   - 无 CRC16
 * PDU 部分（功能码 + 数据）与 RTU 完全一致，寄存器映射逻辑相同。
 */
class ModbusTcpProtocolWorker : public ProtocolBase {
    Q_OBJECT
public:
    explicit ModbusTcpProtocolWorker(QObject *parent = nullptr);
    ~ModbusTcpProtocolWorker() override;

    QString protocolName() const override { return QStringLiteral("ModbusTCP"); }

    struct ModbusTcpConfig {
        QString host = QStringLiteral("127.0.0.1");
        quint16 port = 502;
        quint8  unitId = 1;             // MBAP 中的 Unit Identifier（相当于 RTU 从站 ID）
        int responseTimeoutMs = 1000;
    };

    struct RegisterMapping {
        quint16 address;
        QString name;
        QString unit;
        double scale = 1.0;
        bool signed_ = false;
    };

public slots:
    void scanPorts() override;          // TCP 无端口可扫，直接返回空列表
    void openConnection() override;
    void closeConnection() override;
    void sendCommand(const QByteArray &cmd) override;

    /// 功能码 0x06 写单个寄存器
    void writeRegister(quint16 addr, quint16 value);

    void setMappings(const QVector<RegisterMapping> &m) { m_mappings = m; }
    const QVector<RegisterMapping> &mappings() const { return m_mappings; }

    void setConfig(const ModbusTcpConfig &cfg) { m_cfg = cfg; }
    const ModbusTcpConfig &config() const { return m_cfg; }

protected:
    bool doOpen() override;
    bool doClose() override;
    void doPoll() override;

private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError err);
    void onResponseTimeout();

private:
    /// 构造 MBAP + PDU 请求帧（0x03 读保持寄存器）
    QByteArray makeReadRequest(quint16 startAddr, quint16 count);

    void sendReadRequest(quint16 startAddr, quint16 count);
    void parseResponse(const QByteArray &frame);
    DeviceDataPoint resolveRegister(quint16 addr, quint16 raw) const;

    QTcpSocket *m_socket = nullptr;
    QByteArray  m_rxBuffer;

    ModbusTcpConfig m_cfg;
    QVector<RegisterMapping> m_mappings;

    QTimer  *m_responseTimer = nullptr;
    bool     m_waitingResponse = false;
    quint16 m_transactionId = 0;        // MBAP 事务 ID，每次请求自增
    quint16 m_pollAddr = 0;
    quint16 m_pollCount = 0;
};

#endif // MODBUSTCPPROTOCOLWORKER_H
