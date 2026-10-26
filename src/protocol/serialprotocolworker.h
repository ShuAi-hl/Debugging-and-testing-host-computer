#ifndef SERIALPROTOCOLWORKER_H
#define SERIALPROTOCOLWORKER_H

#include "protocolbase.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QVector>

/**
 * @brief 串口协议 Worker
 *
 * 线程安全：通过 moveToThread 在后台线程运行，UI 只能通过信号/槽与之交互。
 * 支持简单的自定义文本/二进制帧解析（默认帧格式: "$name=value%unit#"）。
 */
class SerialProtocolWorker : public ProtocolBase {
    Q_OBJECT
public:
    explicit SerialProtocolWorker(QObject *parent = nullptr);
    ~SerialProtocolWorker() override;

    QString protocolName() const override { return QStringLiteral("Serial"); }

    /// 串口参数
    struct SerialConfig {
        QString portName;
        qint32 baudRate = 115200;
        QSerialPort::DataBits dataBits = QSerialPort::Data8;
        QSerialPort::Parity parity = QSerialPort::NoParity;
        QSerialPort::StopBits stopBits = QSerialPort::OneStop;
        QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    };

public slots:
    void scanPorts() override;
    void openConnection() override;
    void closeConnection() override;
    void sendCommand(const QByteArray &cmd) override;

    /// 设置/获取配置
    void setConfig(const SerialConfig &cfg) { m_cfg = cfg; }
    const SerialConfig &config() const { return m_cfg; }

protected:
    bool doOpen() override;
    bool doClose() override;

private slots:
    void onReadyRead();
    void onErrorOccurred(QSerialPort::SerialPortError err);

private:
    /// 从原始缓冲区解析完整帧，返回解析到的数据点
    QList<DeviceDataPoint> parseBuffer();

    QSerialPort *m_port = nullptr;
    QByteArray   m_rxBuffer;
    SerialConfig m_cfg;
};

#endif // SERIALPROTOCOLWORKER_H
