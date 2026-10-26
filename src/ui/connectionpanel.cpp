#include "connectionpanel.h"
#include "../core/logger.h"

#include <QSerialPortInfo>
#include <QCanBus>
#include <QDateTime>
#include <QVariantMap>

ConnectionPanel::ConnectionPanel(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

void ConnectionPanel::buildUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    // ========== 顶部第一行：协议选择 ==========
    auto *topRow = new QHBoxLayout();
    m_protocolCombo = new QComboBox(this);
    m_protocolCombo->addItem(QStringLiteral("串口 (Serial)"), static_cast<int>(Protocol::Serial));
    m_protocolCombo->addItem(QStringLiteral("Modbus RTU"), static_cast<int>(Protocol::ModbusRTU));
    m_protocolCombo->addItem(QStringLiteral("Modbus TCP"), static_cast<int>(Protocol::ModbusTCP));
    m_protocolCombo->addItem(QStringLiteral("CAN 总线"), static_cast<int>(Protocol::CAN));
    m_protocolCombo->addItem(QStringLiteral("数据模拟器"), static_cast<int>(Protocol::Simulator));
    m_protocolCombo->setCurrentIndex(4);

    topRow->addWidget(new QLabel(QStringLiteral("协议:"), this));
    topRow->addWidget(m_protocolCombo, 1);
    mainLayout->addLayout(topRow);

    // ========== 顶部第二行：状态 + 操作按钮 ==========
    m_statusLabel = new QLabel(QStringLiteral("● 未连接"), this);
    m_statusLabel->setObjectName(QStringLiteral("statusLabel"));

    m_connectBtn = new QPushButton(QStringLiteral("连接"), this);
    m_disconnectBtn = new QPushButton(QStringLiteral("断开"), this);
    m_disconnectBtn->setEnabled(false);
    m_scanBtn = new QPushButton(QStringLiteral("扫描端口"), this);

    auto *btnRow = new QHBoxLayout();
    btnRow->addWidget(m_statusLabel);
    btnRow->addStretch(1);
    btnRow->addWidget(m_scanBtn);
    btnRow->addWidget(m_connectBtn);
    btnRow->addWidget(m_disconnectBtn);
    mainLayout->addLayout(btnRow);

    // ========== 中间：参数配置 ==========
    m_configStack = new QStackedWidget(this);
    m_configStack->addWidget(createSerialPage());
    m_configStack->addWidget(createModbusPage());
    m_configStack->addWidget(createModbusTcpPage());
    m_configStack->addWidget(createCanPage());
    m_configStack->addWidget(createSimulatorPage());
    mainLayout->addWidget(m_configStack, 1);

    // ========== 通信终端（独立容器，由 MainWindow 放到中央区域） ==========
    m_termGroup = new QGroupBox(QStringLiteral("通信终端"), this);
    auto *termLayout = new QVBoxLayout(m_termGroup);

    m_terminal = new QPlainTextEdit(this);
    m_terminal->setReadOnly(true);
    m_terminal->setMaximumBlockCount(2000);

    auto *rxRow = new QHBoxLayout();
    rxRow->addWidget(new QLabel(QStringLiteral("接收显示:"), this));
    m_rxFormatCombo = new QComboBox(this);
    m_rxFormatCombo->addItem(QStringLiteral("ASCII"));
    m_rxFormatCombo->addItem(QStringLiteral("UTF-8 (中文)"));
    m_rxFormatCombo->addItem(QStringLiteral("HEX"));
    m_rxFormatCombo->setCurrentIndex(1);
    rxRow->addWidget(m_rxFormatCombo);
    rxRow->addStretch(1);

    auto *sendRow = new QHBoxLayout();
    m_sendEdit = new QLineEdit(this);
    m_sendEdit->setPlaceholderText(QStringLiteral("输入要发送的命令..."));
    m_hexModeCheck = new QCheckBox(QStringLiteral("HEX"), this);
    auto *sendBtn = new QPushButton(QStringLiteral("发送"), this);
    sendRow->addWidget(m_sendEdit, 1);
    sendRow->addWidget(m_hexModeCheck);
    sendRow->addWidget(sendBtn);

    termLayout->addLayout(rxRow);
    termLayout->addWidget(m_terminal, 1);
    termLayout->addLayout(sendRow);

    // ========== 连接信号 ==========
    connect(m_protocolCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &ConnectionPanel::onProtocolChanged);
    connect(m_scanBtn, &QPushButton::clicked, this, &ConnectionPanel::onScanClicked);
    connect(m_connectBtn, &QPushButton::clicked, this, &ConnectionPanel::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &ConnectionPanel::onDisconnectClicked);
    connect(sendBtn, &QPushButton::clicked, this, &ConnectionPanel::onSendClicked);
    connect(m_sendEdit, &QLineEdit::returnPressed, this, &ConnectionPanel::onSendClicked);

    onProtocolChanged(m_protocolCombo->currentIndex());
}

// ===== 各协议页面 =====

QWidget *ConnectionPanel::createSerialPage()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);

    m_serialPortCombo = new QComboBox(page);
    m_serialPortCombo->setEditable(false);
    m_serialPortCombo->setPlaceholderText(QStringLiteral("← 点击「扫描端口」"));

    m_baudCombo = new QComboBox(page);
    m_baudCombo->addItems({QStringLiteral("9600"), QStringLiteral("19200"),
                           QStringLiteral("38400"), QStringLiteral("57600"),
                           QStringLiteral("115200"), QStringLiteral("230400")});
    m_baudCombo->setCurrentText(QStringLiteral("115200"));

    m_dataBitsCombo = new QComboBox(page);
    m_dataBitsCombo->addItems({QStringLiteral("8"), QStringLiteral("7")});

    m_parityCombo = new QComboBox(page);
    m_parityCombo->addItems({QStringLiteral("无"), QStringLiteral("偶"), QStringLiteral("奇")});

    m_stopBitsCombo = new QComboBox(page);
    m_stopBitsCombo->addItems({QStringLiteral("1"), QStringLiteral("1.5"), QStringLiteral("2")});

    form->addRow(QStringLiteral("端口:"), m_serialPortCombo);
    form->addRow(QStringLiteral("波特率:"), m_baudCombo);
    form->addRow(QStringLiteral("数据位:"), m_dataBitsCombo);
    form->addRow(QStringLiteral("校验位:"), m_parityCombo);
    form->addRow(QStringLiteral("停止位:"), m_stopBitsCombo);

    return page;
}

QWidget *ConnectionPanel::createModbusPage()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);

    m_modbusPortCombo = new QComboBox(page);
    m_modbusPortCombo->setEditable(false);
    m_modbusPortCombo->setPlaceholderText(QStringLiteral("← 点击「扫描端口」"));

    m_modbusBaudCombo = new QComboBox(page);
    m_modbusBaudCombo->addItems({QStringLiteral("9600"), QStringLiteral("19200"),
                                 QStringLiteral("38400"), QStringLiteral("57600"),
                                 QStringLiteral("115200")});
    m_modbusBaudCombo->setCurrentText(QStringLiteral("9600"));

    m_slaveIdSpin = new QSpinBox(page);
    m_slaveIdSpin->setRange(1, 247);
    m_slaveIdSpin->setValue(1);

    m_pollIntervalSpin = new QSpinBox(page);
    m_pollIntervalSpin->setRange(100, 5000);
    m_pollIntervalSpin->setSingleStep(100);
    m_pollIntervalSpin->setValue(1000);
    m_pollIntervalSpin->setSuffix(QStringLiteral(" ms"));

    form->addRow(QStringLiteral("串口:"), m_modbusPortCombo);
    form->addRow(QStringLiteral("波特率:"), m_modbusBaudCombo);
    form->addRow(QStringLiteral("从站 ID:"), m_slaveIdSpin);
    form->addRow(QStringLiteral("轮询周期:"), m_pollIntervalSpin);

    return page;
}

QWidget *ConnectionPanel::createModbusTcpPage()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);

    m_tcpHostEdit = new QLineEdit(page);
    m_tcpHostEdit->setPlaceholderText(QStringLiteral("例如 192.168.1.10"));
    m_tcpHostEdit->setText(QStringLiteral("127.0.0.1"));

    m_tcpPortSpin = new QSpinBox(page);
    m_tcpPortSpin->setRange(1, 65535);
    m_tcpPortSpin->setValue(502);

    m_tcpUnitIdSpin = new QSpinBox(page);
    m_tcpUnitIdSpin->setRange(0, 255);
    m_tcpUnitIdSpin->setValue(1);

    m_tcpPollIntervalSpin = new QSpinBox(page);
    m_tcpPollIntervalSpin->setRange(100, 5000);
    m_tcpPollIntervalSpin->setSingleStep(100);
    m_tcpPollIntervalSpin->setValue(1000);
    m_tcpPollIntervalSpin->setSuffix(QStringLiteral(" ms"));

    form->addRow(QStringLiteral("目标 IP:"), m_tcpHostEdit);
    form->addRow(QStringLiteral("端口:"), m_tcpPortSpin);
    form->addRow(QStringLiteral("Unit ID:"), m_tcpUnitIdSpin);
    form->addRow(QStringLiteral("轮询周期:"), m_tcpPollIntervalSpin);

    return page;
}

QWidget *ConnectionPanel::createCanPage()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);

    m_canPluginCombo = new QComboBox(page);
    m_canPluginCombo->setEditable(false);
    m_canPluginCombo->addItems(QCanBus::instance()->plugins());

    m_canDeviceCombo = new QComboBox(page);
    m_canDeviceCombo->setEditable(false);
    m_canDeviceCombo->setPlaceholderText(QStringLiteral("← 点击「扫描端口」"));

    m_canBitrateSpin = new QSpinBox(page);
    m_canBitrateSpin->setRange(1000, 1000000);
    m_canBitrateSpin->setSingleStep(1000);
    m_canBitrateSpin->setValue(500000);
    m_canBitrateSpin->setSuffix(QStringLiteral(" bps"));

    form->addRow(QStringLiteral("插件:"), m_canPluginCombo);
    form->addRow(QStringLiteral("设备:"), m_canDeviceCombo);
    form->addRow(QStringLiteral("波特率:"), m_canBitrateSpin);

    return page;
}

QWidget *ConnectionPanel::createSimulatorPage()
{
    auto *page = new QWidget(this);
    auto *form = new QFormLayout(page);

    m_simSceneCombo = new QComboBox(page);
    m_simSceneCombo->addItem(QStringLiteral("基础正弦波动"), 0);
    m_simSceneCombo->addItem(QStringLiteral("电机启停曲线"), 1);
    m_simSceneCombo->addItem(QStringLiteral("加热炉温控"), 2);

    form->addRow(QStringLiteral("模拟场景:"), m_simSceneCombo);

    return page;
}

// ===== Protocol + Config Getters =====

Protocol ConnectionPanel::currentProtocol() const
{
    return static_cast<Protocol>(m_protocolCombo->currentData().toInt());
}

QVariantMap ConnectionPanel::serialConfig() const
{
    QVariantMap m;
    m[QStringLiteral("portName")] = m_serialPortCombo->currentData().toString();
    m[QStringLiteral("baudRate")] = m_baudCombo->currentText().toInt();
    m[QStringLiteral("dataBits")] = (m_dataBitsCombo->currentText() == QStringLiteral("7")) ? 7 : 8;
    QString parity = m_parityCombo->currentText();
    m[QStringLiteral("parity")] = parity;
    m[QStringLiteral("stopBits")] = m_stopBitsCombo->currentText();
    return m;
}

QVariantMap ConnectionPanel::modbusConfig() const
{
    QVariantMap m;
    m[QStringLiteral("portName")] = m_modbusPortCombo->currentData().toString();
    m[QStringLiteral("baudRate")] = m_modbusBaudCombo->currentText().toInt();
    m[QStringLiteral("slaveId")] = m_slaveIdSpin->value();
    m[QStringLiteral("pollInterval")] = m_pollIntervalSpin->value();
    return m;
}

QVariantMap ConnectionPanel::modbusTcpConfig() const
{
    QVariantMap m;
    m[QStringLiteral("host")] = m_tcpHostEdit->text().trimmed();
    m[QStringLiteral("port")] = m_tcpPortSpin->value();
    m[QStringLiteral("unitId")] = m_tcpUnitIdSpin->value();
    m[QStringLiteral("pollInterval")] = m_tcpPollIntervalSpin->value();
    return m;
}

QVariantMap ConnectionPanel::canConfig() const
{
    QVariantMap m;
    const QString devData = m_canDeviceCombo->currentData().toString();
    const int slash = devData.indexOf(QLatin1Char('/'));
    if (slash > 0) {
        m[QStringLiteral("pluginName")] = devData.left(slash);
        m[QStringLiteral("deviceName")] = devData.mid(slash + 1);
    } else {
        m[QStringLiteral("pluginName")] = m_canPluginCombo->currentText();
        m[QStringLiteral("deviceName")] = devData;
    }
    m[QStringLiteral("bitrate")] = m_canBitrateSpin->value();
    return m;
}

QVariantMap ConnectionPanel::simConfig() const
{
    QVariantMap m;
    m[QStringLiteral("scene")] = m_simSceneCombo->currentData().toInt();
    m[QStringLiteral("pollInterval")] = 500;
    return m;
}

// ===== View slots（只发信号，不碰 Worker）=====

void ConnectionPanel::onProtocolChanged(int idx)
{
    Q_UNUSED(idx);
    Protocol p = currentProtocol();
    m_configStack->setCurrentIndex(static_cast<int>(p));
    // TCP/模拟器不需要扫描，只有串口类协议才发扫描请求
    if (p == Protocol::Serial || p == Protocol::ModbusRTU || p == Protocol::CAN)
        emit scanRequested(static_cast<int>(p));
}

void ConnectionPanel::onScanClicked()
{
    emit scanRequested(static_cast<int>(currentProtocol()));
}

void ConnectionPanel::onConnectClicked()
{
    emit connectRequested(static_cast<int>(currentProtocol()));
}

void ConnectionPanel::onDisconnectClicked()
{
    emit disconnectRequested(static_cast<int>(currentProtocol()));
}

void ConnectionPanel::onSendClicked()
{
    QString text = m_sendEdit->text();
    if (text.isEmpty()) return;

    QByteArray cmd;
    if (m_hexModeCheck->isChecked()) {
        cmd = QByteArray::fromHex(text.toUtf8());
    } else {
        cmd = text.toUtf8();
    }

    Protocol p = currentProtocol();
    if (p != Protocol::Simulator) {
        emit sendRequested(static_cast<int>(p), cmd);
    }

    // 终端显示（属于 View 自身职责）
    m_terminal->appendPlainText(
        QStringLiteral("[TX] %1").arg(m_hexModeCheck->isChecked()
                                        ? QString::fromLatin1(cmd.toHex(' '))
                                        : QString::fromUtf8(cmd)));
}

// ===== Controller 回调 → 更新 View =====

void ConnectionPanel::onPortsScanned(const QStringList &ports)
{
    auto fillCombo = [&ports](QComboBox *combo) {
        combo->clear();
        for (const auto &entry : ports) {
            const auto parts = entry.split(QLatin1Char('|'));
            if (parts.size() >= 2)
                combo->addItem(parts.last(), parts.first());
            else
                combo->addItem(entry, entry);
        }
    };

    Protocol p = currentProtocol();
    switch (p) {
    case Protocol::Serial:    fillCombo(m_serialPortCombo); break;
    case Protocol::ModbusRTU: fillCombo(m_modbusPortCombo); break;
    case Protocol::CAN:
        m_canDeviceCombo->clear();
        for (const auto &entry : ports) {
            const auto parts = entry.split(QLatin1Char('|'));
            if (parts.size() == 3) {
                m_canDeviceCombo->addItem(parts[2], parts[0] + QLatin1Char('/') + parts[1]);
            } else {
                m_canDeviceCombo->addItem(entry, entry);
            }
        }
        break;
    case Protocol::ModbusTCP:
        // TCP 无需扫描，直接返回
        return;
    case Protocol::Simulator: break;
    }

    m_terminal->appendPlainText(
        QStringLiteral("[INFO] 扫描到 %1 个设备").arg(ports.size()));
}

void ConnectionPanel::onConnectionStateChanged(bool connected)
{
    if (connected) {
        m_statusLabel->setText(QStringLiteral("● 已连接"));
        m_statusLabel->setObjectName(QStringLiteral("statusLabelConnected"));
        m_connectBtn->setEnabled(false);
        m_disconnectBtn->setEnabled(true);
    } else {
        m_statusLabel->setText(QStringLiteral("● 未连接"));
        m_statusLabel->setObjectName(QStringLiteral("statusLabel"));
        m_connectBtn->setEnabled(true);
        m_disconnectBtn->setEnabled(false);
    }
    m_statusLabel->style()->unpolish(m_statusLabel);
    m_statusLabel->style()->polish(m_statusLabel);

    m_terminal->appendPlainText(
        QStringLiteral("[INFO] 连接状态: %1")
            .arg(connected ? QStringLiteral("已连接") : QStringLiteral("已断开")));
}

void ConnectionPanel::onErrorOccurred(const QString &msg)
{
    m_terminal->appendPlainText(
        QStringLiteral("[ERROR] %1 %2")
            .arg(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss")), msg));
}

void ConnectionPanel::onRawBytesReceived(const QByteArray &data)
{
    const int fmt = m_rxFormatCombo ? m_rxFormatCombo->currentIndex() : 1;
    QString text;

    if (fmt == 2) {
        m_rxPendingBytes.clear();
        text = QString::fromLatin1(data.toHex(' ')).toUpper();
    } else if (fmt == 0) {
        m_rxPendingBytes.clear();
        QByteArray printable;
        printable.reserve(data.size());
        for (char c : data) {
            unsigned char uc = static_cast<unsigned char>(c);
            printable.append((uc >= 0x20 && uc < 0x7F) ? c : '.');
        }
        text = QString::fromLatin1(printable);
    } else {
        QByteArray buf = m_rxPendingBytes + data;
        m_rxPendingBytes.clear();

        int end = buf.size();
        for (int i = buf.size() - 1; i >= 0 && i >= buf.size() - 3; --i) {
            unsigned char uc = static_cast<unsigned char>(buf.at(i));
            if ((uc & 0x80) == 0) break;
            if ((uc & 0xC0) == 0xC0) {
                int need = ((uc & 0xE0) == 0xC0) ? 2 :
                           ((uc & 0xF0) == 0xE0) ? 3 :
                           ((uc & 0xF8) == 0xF0) ? 4 : 1;
                if (buf.size() - i < need) {
                    m_rxPendingBytes = buf.mid(i);
                    end = i;
                }
                break;
            }
        }
        text = QString::fromUtf8(buf.constData(), end);
    }

    if (!text.isEmpty())
        m_terminal->appendPlainText(QStringLiteral("[RX] %1").arg(text));
}
