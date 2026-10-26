#ifndef CONNECTIONPANEL_H
#define CONNECTIONPANEL_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QSerialPort>

/**
 * @brief 协议枚举（暴露给 Controller 使用）
 */
enum class Protocol { Serial = 0, ModbusRTU = 1, ModbusTCP = 2, CAN = 3, Simulator = 4 };

/**
 * @brief 连接面板 —— 纯 View，不依赖任何 Worker
 *
 * 只负责：绘制 UI、收发信号、内部状态切换。
 * 所有业务逻辑（连接/断开/扫描/发送）通过信号委托给 Controller。
 */
class ConnectionPanel : public QWidget {
    Q_OBJECT
public:
    explicit ConnectionPanel(QWidget *parent = nullptr);
    ~ConnectionPanel() override = default;

    /// 当前选择的协议
    Protocol currentProtocol() const;

    /// 各协议配置 getter（Controller 读取后下发给 Worker）
    QVariantMap serialConfig() const;   // { portName, baudRate, dataBits, parity, stopBits }
    QVariantMap modbusConfig() const;   // { portName, baudRate, slaveId, pollInterval }
    QVariantMap modbusTcpConfig() const;// { host, port, unitId, pollInterval }
    QVariantMap canConfig() const;      // { pluginName, deviceName, bitrate }
    QVariantMap simConfig() const;      // { scene, pollInterval }

    /// 通信终端组件（由 MainWindow 取出放到中央区域）
    QWidget *terminalWidget() const { return m_termGroup; }

signals:
    void scanRequested(int protocol);
    void connectRequested(int protocol);
    void disconnectRequested(int protocol);
    void sendRequested(int protocol, const QByteArray &data);

private slots:
    void onProtocolChanged(int idx);
    void onScanClicked();
    void onConnectClicked();
    void onDisconnectClicked();
    void onSendClicked();

public slots:
    // Controller 回调 → 更新 View 显示
    void onPortsScanned(const QStringList &ports);
    void onConnectionStateChanged(bool connected);
    void onErrorOccurred(const QString &msg);
    void onRawBytesReceived(const QByteArray &data);

private:
    void buildUi();
    QWidget *createSerialPage();
    QWidget *createModbusPage();
    QWidget *createModbusTcpPage();
    QWidget *createCanPage();
    QWidget *createSimulatorPage();

    // UI 控件
    QComboBox *m_protocolCombo = nullptr;
    QLabel    *m_statusLabel = nullptr;
    QPushButton *m_connectBtn = nullptr;
    QPushButton *m_disconnectBtn = nullptr;
    QPushButton *m_scanBtn = nullptr;
    QStackedWidget *m_configStack = nullptr;

    // Serial
    QComboBox *m_serialPortCombo = nullptr;
    QComboBox *m_baudCombo = nullptr;
    QComboBox *m_dataBitsCombo = nullptr;
    QComboBox *m_parityCombo = nullptr;
    QComboBox *m_stopBitsCombo = nullptr;

    // Modbus RTU
    QComboBox *m_modbusPortCombo = nullptr;
    QComboBox *m_modbusBaudCombo = nullptr;
    QSpinBox  *m_slaveIdSpin = nullptr;
    QSpinBox  *m_pollIntervalSpin = nullptr;

    // Modbus TCP
    QLineEdit *m_tcpHostEdit = nullptr;
    QSpinBox  *m_tcpPortSpin = nullptr;
    QSpinBox  *m_tcpUnitIdSpin = nullptr;
    QSpinBox  *m_tcpPollIntervalSpin = nullptr;

    // CAN
    QComboBox *m_canPluginCombo = nullptr;
    QComboBox *m_canDeviceCombo = nullptr;
    QSpinBox  *m_canBitrateSpin = nullptr;

    // Simulator
    QComboBox *m_simSceneCombo = nullptr;

    // 调试终端
    QGroupBox      *m_termGroup = nullptr;
    QPlainTextEdit *m_terminal = nullptr;
    QLineEdit      *m_sendEdit = nullptr;
    QCheckBox      *m_hexModeCheck = nullptr;
    QComboBox      *m_rxFormatCombo = nullptr;
    QByteArray      m_rxPendingBytes;
};

#endif // CONNECTIONPANEL_H
