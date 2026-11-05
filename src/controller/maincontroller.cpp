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

#include <QMetaObject>
#include <utility>

namespace {
/// 把操作投递到 Worker 所属线程执行。
/// Worker 的定时器/套接字只能在它自己的线程里操作，GUI 线程直接调用会失效。
template <typename Fn>
void postToWorker(QObject *worker, Fn &&fn)
{
    QMetaObject::invokeMethod(worker, std::forward<Fn>(fn), Qt::QueuedConnection);
}
} // namespace

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
    auto wireWorkerToView = [&](IProtocolWorker *w, int protocol) {
        connect(w, &IProtocolWorker::portsScanned,
                m_connPanel, &ConnectionPanel::onPortsScanned);
        // 带上协议编号，View 才能按协议分别记录连接状态
        connect(w, &IProtocolWorker::connectionStateChanged,
                m_connPanel, [panel = m_connPanel, protocol](bool connected) {
                    panel->onConnectionStateChanged(protocol, connected);
                });
        connect(w, &IProtocolWorker::errorOccurred,
                m_connPanel, &ConnectionPanel::onErrorOccurred);
        connect(w, &IProtocolWorker::rawBytesReceived,
                m_connPanel, &ConnectionPanel::onRawBytesReceived);
    };
    wireWorkerToView(m_serialWorker,    static_cast<int>(Protocol::Serial));
    wireWorkerToView(m_modbusWorker,    static_cast<int>(Protocol::ModbusRTU));
    wireWorkerToView(m_modbusTcpWorker, static_cast<int>(Protocol::ModbusTCP));
    wireWorkerToView(m_canWorker,       static_cast<int>(Protocol::CAN));
    wireWorkerToView(m_simWorker,       static_cast<int>(Protocol::Simulator));

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
    case Protocol::Serial:
        postToWorker(m_serialWorker, [w = m_serialWorker] { w->scanPorts(); });
        break;
    case Protocol::ModbusRTU:
        postToWorker(m_modbusWorker, [w = m_modbusWorker] { w->scanPorts(); });
        break;
    case Protocol::ModbusTCP:
        postToWorker(m_modbusTcpWorker, [w = m_modbusTcpWorker] { w->scanPorts(); });
        break;
    case Protocol::CAN:
        postToWorker(m_canWorker, [w = m_canWorker] { w->scanPorts(); });
        break;
    case Protocol::Simulator:
        postToWorker(m_simWorker, [w = m_simWorker] { w->scanPorts(); });
        break;
    }
}

void MainController::onConnectRequested(int protocol)
{
    // 先把配置投递到 Worker 线程，再投递打开连接；同一线程队列保证先后顺序
    applyConfig(protocol);
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:
        postToWorker(m_serialWorker, [w = m_serialWorker] { w->openConnection(); });
        break;
    case Protocol::ModbusRTU:
        postToWorker(m_modbusWorker, [w = m_modbusWorker] { w->openConnection(); });
        break;
    case Protocol::ModbusTCP:
        postToWorker(m_modbusTcpWorker, [w = m_modbusTcpWorker] { w->openConnection(); });
        break;
    case Protocol::CAN:
        postToWorker(m_canWorker, [w = m_canWorker] { w->openConnection(); });
        break;
    case Protocol::Simulator:
        postToWorker(m_simWorker, [w = m_simWorker] { w->openConnection(); });
        break;
    }
}

void MainController::onDisconnectRequested()
{
    // 「断开」是全局动作：所有 Worker 一律关闭，避免后台残留连接继续跑
    postToWorker(m_serialWorker,
                 [w = m_serialWorker] { w->closeConnection(); });
    postToWorker(m_modbusWorker,
                 [w = m_modbusWorker] { w->closeConnection(); });
    postToWorker(m_modbusTcpWorker,
                 [w = m_modbusTcpWorker] { w->closeConnection(); });
    postToWorker(m_canWorker,
                 [w = m_canWorker] { w->closeConnection(); });
    postToWorker(m_simWorker,
                 [w = m_simWorker] { w->closeConnection(); });
}

void MainController::onSendRequested(int protocol, const QByteArray &data)
{
    switch (static_cast<Protocol>(protocol)) {
    case Protocol::Serial:
        postToWorker(m_serialWorker,
                     [w = m_serialWorker, data] { w->sendCommand(data); });
        break;
    case Protocol::ModbusRTU:
        postToWorker(m_modbusWorker,
                     [w = m_modbusWorker, data] { w->sendCommand(data); });
        break;
    case Protocol::ModbusTCP:
        postToWorker(m_modbusTcpWorker,
                     [w = m_modbusTcpWorker, data] { w->sendCommand(data); });
        break;
    case Protocol::CAN:
        postToWorker(m_canWorker,
                     [w = m_canWorker, data] { w->sendCommand(data); });
        break;
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
        postToWorker(m_serialWorker,
                     [w = m_serialWorker, cfg] { w->setConfig(cfg); });
        break;
    }
    case Protocol::ModbusRTU: {
        auto map = m_connPanel->modbusConfig();
        ModbusRtuProtocolWorker::ModbusConfig cfg;
        cfg.portName = map[QStringLiteral("portName")].toString();
        cfg.baudRate = map[QStringLiteral("baudRate")].value<qint32>();
        cfg.slaveId  = map[QStringLiteral("slaveId")].value<quint8>();
        const int pollMs = map[QStringLiteral("pollInterval")].toInt();
        postToWorker(m_modbusWorker,
                     [w = m_modbusWorker, cfg, pollMs] {
                         w->setConfig(cfg);
                         w->setPollInterval(pollMs);
                     });
        break;
    }
    case Protocol::ModbusTCP: {
        auto map = m_connPanel->modbusTcpConfig();
        ModbusTcpProtocolWorker::ModbusTcpConfig cfg;
        cfg.host   = map[QStringLiteral("host")].toString();
        cfg.port   = map[QStringLiteral("port")].value<quint16>();
        cfg.unitId = map[QStringLiteral("unitId")].value<quint8>();
        const int pollMs = map[QStringLiteral("pollInterval")].toInt();
        postToWorker(m_modbusTcpWorker,
                     [w = m_modbusTcpWorker, cfg, pollMs] {
                         w->setConfig(cfg);
                         w->setPollInterval(pollMs);
                     });
        break;
    }
    case Protocol::CAN: {
        auto map = m_connPanel->canConfig();
        CanProtocolWorker::CanConfig cfg;
        cfg.pluginName = map[QStringLiteral("pluginName")].toString();
        cfg.deviceName = map[QStringLiteral("deviceName")].toString();
        cfg.bitrate    = map[QStringLiteral("bitrate")].toInt();
        postToWorker(m_canWorker,
                     [w = m_canWorker, cfg] { w->setConfig(cfg); });
        break;
    }
    case Protocol::Simulator: {
        auto map = m_connPanel->simConfig();
        const int scene = map[QStringLiteral("scene")].toInt();
        const int pollMs = map[QStringLiteral("pollInterval")].toInt();
        postToWorker(m_simWorker,
                     [w = m_simWorker, scene, pollMs] {
                         w->setScene(static_cast<SimulatorWorker::Scene>(scene));
                         w->setPollInterval(pollMs);
                     });
        break;
    }
    }
}
