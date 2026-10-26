#include "maincontroller.h"
#include "../core/databus.h"
#include "../core/logger.h"
#include "../protocol/serialprotocolworker.h"
#include "../protocol/modbusrtuprotocolworker.h"
#include "../protocol/modbustcpprotocolworker.h"
#include "../protocol/canprotocolworker.h"
#include "../protocol/simulatorworker.h"
#include "../ui/connectionpanel.h"
#include "../ui/datatablewidget.h"
#include "../ui/realtimechartwidget.h"

MainController::MainController(QObject *parent)
    : QObject(parent)
{
    // 创建 Worker 并 moveToThread
    m_serialWorker = new SerialProtocolWorker();
    m_serThread = new QThread(this);
    m_serialWorker->moveToThread(m_serThread);
    connect(m_serThread, &QThread::finished, m_serialWorker, &QObject::deleteLater);

    m_modbusWorker = new ModbusRtuProtocolWorker();
    m_modThread = new QThread(this);
    m_modbusWorker->moveToThread(m_modThread);
    connect(m_modThread, &QThread::finished, m_modbusWorker, &QObject::deleteLater);

    m_modbusTcpWorker = new ModbusTcpProtocolWorker();
    m_modTcpThread = new QThread(this);
    m_modbusTcpWorker->moveToThread(m_modTcpThread);
    connect(m_modTcpThread, &QThread::finished, m_modbusTcpWorker, &QObject::deleteLater);

    m_canWorker = new CanProtocolWorker();
    m_canThread = new QThread(this);
    m_canWorker->moveToThread(m_canThread);
    connect(m_canThread, &QThread::finished, m_canWorker, &QObject::deleteLater);

    m_simWorker = new SimulatorWorker();
    m_simThread = new QThread(this);
    m_simWorker->moveToThread(m_simThread);
    connect(m_simThread, &QThread::finished, m_simWorker, &QObject::deleteLater);
}

MainController::~MainController()
{
    stopWorkers();
}

void MainController::startWorkers()
{
    m_serThread->start();
    m_modThread->start();
    m_modTcpThread->start();
    m_canThread->start();
    m_simThread->start();
}

void MainController::stopWorkers()
{
    if (m_serThread)     { m_serThread->quit();     m_serThread->wait();     }
    if (m_modThread)     { m_modThread->quit();     m_modThread->wait();     }
    if (m_modTcpThread)  { m_modTcpThread->quit();  m_modTcpThread->wait();  }
    if (m_canThread)     { m_canThread->quit();     m_canThread->wait();     }
    if (m_simThread)     { m_simThread->quit();     m_simThread->wait();     }
}

void MainController::setViews(ConnectionPanel *panel,
                              RealtimeChartWidget *chart,
                              DataTableWidget *table)
{
    m_connPanel   = panel;
    m_chartWidget = chart;
    m_tableWidget = table;
    wireSignals();
}

void MainController::wireSignals()
{
    if (!m_connPanel) return;
    auto &bus = DataBus::instance();

    // ===== A. View 请求 → Controller =====
    connect(m_connPanel, &ConnectionPanel::scanRequested,
            this, &MainController::onScanRequested);
    connect(m_connPanel, &ConnectionPanel::connectRequested,
            this, &MainController::onConnectRequested);
    connect(m_connPanel, &ConnectionPanel::disconnectRequested,
            this, &MainController::onDisconnectRequested);
    connect(m_connPanel, &ConnectionPanel::sendRequested,
            this, &MainController::onSendRequested);

    // ===== B. Worker → DataBus（数据发布，跨线程自动 QueuedConnection）=====
    auto wireWorkerToBus = [&](IProtocolWorker *w) {
        connect(w, &IProtocolWorker::dataParsed,
                &bus, &DataBus::publish);
        connect(w, &IProtocolWorker::batchDataParsed,
                &bus, &DataBus::publishBatch);
    };
    wireWorkerToBus(m_serialWorker);
    wireWorkerToBus(m_modbusWorker);
    wireWorkerToBus(m_modbusTcpWorker);
    wireWorkerToBus(m_canWorker);
    wireWorkerToBus(m_simWorker);

    // ===== C. Worker → View（状态/调试信号）=====
    auto wireWorkerToView = [&](IProtocolWorker *w) {
        connect(w, &IProtocolWorker::portsScanned,
                m_connPanel, &ConnectionPanel::onPortsScanned);
        connect(w, &IProtocolWorker::connectionStateChanged,
                m_connPanel, &ConnectionPanel::onConnectionStateChanged);
        connect(w, &IProtocolWorker::errorOccurred,
                m_connPanel, &ConnectionPanel::onErrorOccurred);
        connect(w, &IProtocolWorker::rawBytesReceived,
                m_connPanel, &ConnectionPanel::onRawBytesReceived);
    };
    wireWorkerToView(m_serialWorker);
    wireWorkerToView(m_modbusWorker);
    wireWorkerToView(m_modbusTcpWorker);
    wireWorkerToView(m_canWorker);
    wireWorkerToView(m_simWorker);

    // ===== D. DataBus → UI 消费者 =====
    if (m_chartWidget) {
        connect(&bus, &DataBus::dataConsumed,
                m_chartWidget, &RealtimeChartWidget::onDataConsumed);
        connect(&bus, &DataBus::batchConsumed,
                m_chartWidget, &RealtimeChartWidget::onBatchConsumed);
    }
    if (m_tableWidget) {
        connect(&bus, &DataBus::dataConsumed,
                m_tableWidget, &DataTableWidget::onDataConsumed);
    }
}

// ===== Controller slots =====

void MainController::onScanRequested(int protocol)
{
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:    m_serialWorker->scanPorts();      break;
    case Protocol::ModbusRTU: m_modbusWorker->scanPorts();      break;
    case Protocol::ModbusTCP: m_modbusTcpWorker->scanPorts();   break;
    case Protocol::CAN:       m_canWorker->scanPorts();         break;
    case Protocol::Simulator: m_simWorker->scanPorts();         break;
    }
}

void MainController::onConnectRequested(int protocol)
{
    applyConfig(protocol);
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:    m_serialWorker->openConnection();      break;
    case Protocol::ModbusRTU: m_modbusWorker->openConnection();      break;
    case Protocol::ModbusTCP: m_modbusTcpWorker->openConnection();   break;
    case Protocol::CAN:       m_canWorker->openConnection();         break;
    case Protocol::Simulator: m_simWorker->openConnection();         break;
    }
}

void MainController::onDisconnectRequested(int protocol)
{
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:    m_serialWorker->closeConnection();      break;
    case Protocol::ModbusRTU: m_modbusWorker->closeConnection();      break;
    case Protocol::ModbusTCP: m_modbusTcpWorker->closeConnection();   break;
    case Protocol::CAN:       m_canWorker->closeConnection();         break;
    case Protocol::Simulator: m_simWorker->closeConnection();         break;
    }
}

void MainController::onSendRequested(int protocol, const QByteArray &data)
{
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:    m_serialWorker->sendCommand(data);      break;
    case Protocol::ModbusRTU: m_modbusWorker->sendCommand(data);      break;
    case Protocol::ModbusTCP: m_modbusTcpWorker->sendCommand(data);   break;
    case Protocol::CAN:       m_canWorker->sendCommand(data);         break;
    case Protocol::Simulator: break;
    }
}

// ===== 配置下发（读 View getter → 组装 Config → Worker::setConfig）=====

void MainController::applyConfig(int protocol)
{
    if (!m_connPanel) return;

    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial: {
        auto map = m_connPanel->serialConfig();
        SerialProtocolWorker::SerialConfig cfg;
        cfg.portName = map[QStringLiteral("portName")].toString();
        cfg.baudRate = map[QStringLiteral("baudRate")].toInt();
        cfg.dataBits = map[QStringLiteral("dataBits")].toInt() == 7
                       ? QSerialPort::Data7 : QSerialPort::Data8;
        QString parity = map[QStringLiteral("parity")].toString();
        if (parity == QStringLiteral("无")) cfg.parity = QSerialPort::NoParity;
        else if (parity == QStringLiteral("偶")) cfg.parity = QSerialPort::EvenParity;
        else cfg.parity = QSerialPort::OddParity;
        QString sb = map[QStringLiteral("stopBits")].toString();
        if (sb == QStringLiteral("1")) cfg.stopBits = QSerialPort::OneStop;
        else if (sb == QStringLiteral("2")) cfg.stopBits = QSerialPort::TwoStop;
        else cfg.stopBits = QSerialPort::OneAndHalfStop;
        m_serialWorker->setConfig(cfg);
        break;
    }
    case Protocol::ModbusRTU: {
        auto map = m_connPanel->modbusConfig();
        ModbusRtuProtocolWorker::ModbusConfig cfg;
        cfg.portName = map[QStringLiteral("portName")].toString();
        cfg.baudRate = map[QStringLiteral("baudRate")].value<qint32>();
        cfg.slaveId  = map[QStringLiteral("slaveId")].value<quint8>();
        m_modbusWorker->setConfig(cfg);
        m_modbusWorker->setPollInterval(map[QStringLiteral("pollInterval")].toInt());
        break;
    }
    case Protocol::ModbusTCP: {
        auto map = m_connPanel->modbusTcpConfig();
        ModbusTcpProtocolWorker::ModbusTcpConfig cfg;
        cfg.host   = map[QStringLiteral("host")].toString();
        cfg.port   = map[QStringLiteral("port")].value<quint16>();
        cfg.unitId = map[QStringLiteral("unitId")].value<quint8>();
        m_modbusTcpWorker->setConfig(cfg);
        m_modbusTcpWorker->setPollInterval(map[QStringLiteral("pollInterval")].toInt());
        break;
    }
    case Protocol::CAN: {
        auto map = m_connPanel->canConfig();
        CanProtocolWorker::CanConfig cfg;
        cfg.pluginName = map[QStringLiteral("pluginName")].toString();
        cfg.deviceName = map[QStringLiteral("deviceName")].toString();
        cfg.bitrate    = map[QStringLiteral("bitrate")].toInt();
        m_canWorker->setConfig(cfg);
        break;
    }
    case Protocol::Simulator: {
        auto map = m_connPanel->simConfig();
        m_simWorker->setScene(static_cast<SimulatorWorker::Scene>(
                                  map[QStringLiteral("scene")].toInt()));
        m_simWorker->setPollInterval(map[QStringLiteral("pollInterval")].toInt());
        break;
    }
    }
}
