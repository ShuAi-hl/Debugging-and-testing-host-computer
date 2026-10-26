#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QThread>

class SerialProtocolWorker;
class ModbusRtuProtocolWorker;
class ModbusTcpProtocolWorker;
class CanProtocolWorker;
class SimulatorWorker;

class ConnectionPanel;
class RealtimeChartWidget;
class DataTableWidget;

/**
 * @brief Controller 层 —— 业务路由 + Worker 生命周期管理
 *
 * 职责：
 *   1) 持有所有 Worker 和线程，负责创建、moveToThread、quit+wait
 *   2) 持有 View 指针，读 View getter 拿配置
 *   3) 信号路由：View 请求 → Controller → Worker；Worker 状态 → Controller → View
 *   4) 配置下发：组装 Config struct 后调 Worker::setConfig
 *
 * 不做的事：
 *   - 不创建 UI 控件（那是 View 的活）
 *   - 不处理业务逻辑细节（Worker 自己的业务逻辑在 Model 层）
 *   - 不管理窗口布局（那是 MainWindow 的活）
 */
class MainController : public QObject {
    Q_OBJECT
public:
    explicit MainController(QObject *parent = nullptr);
    ~MainController() override;

    /// 注入 View（MainWindow 创建后调用）
    void setViews(ConnectionPanel *panel,
                  RealtimeChartWidget *chart,
                  DataTableWidget *table);

    /// 启动所有 Worker 线程（在 setViews 之后调用）
    void startWorkers();

    /// 关闭所有 Worker 线程（MainWindow closeEvent 中调用）
    void stopWorkers();

private slots:
    // ===== View 请求 → Worker =====
    void onScanRequested(int protocol);
    void onConnectRequested(int protocol);
    void onDisconnectRequested(int protocol);
    void onSendRequested(int protocol, const QByteArray &data);

private:
    void wireSignals();
    void applyConfig(int protocol);

    // ===== Workers + Threads（Controller 独占）=====
    SerialProtocolWorker    *m_serialWorker   = nullptr;
    ModbusRtuProtocolWorker *m_modbusWorker   = nullptr;
    ModbusTcpProtocolWorker *m_modbusTcpWorker = nullptr;
    CanProtocolWorker       *m_canWorker      = nullptr;
    SimulatorWorker         *m_simWorker      = nullptr;

    QThread *m_serThread     = nullptr;
    QThread *m_modThread     = nullptr;
    QThread *m_modTcpThread  = nullptr;
    QThread *m_canThread     = nullptr;
    QThread *m_simThread     = nullptr;

    // ===== View 指针（Controller 读 getter、连回调）=====
    ConnectionPanel     *m_connPanel   = nullptr;
    RealtimeChartWidget *m_chartWidget = nullptr;
    DataTableWidget     *m_tableWidget = nullptr;
};

#endif // MAINCONTROLLER_H
