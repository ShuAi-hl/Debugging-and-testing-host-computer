#include "mainwindow.h"
#include "core/logger.h"
#include "controller/maincontroller.h"
#include "ui/connectionpanel.h"
#include "ui/datatablewidget.h"
#include "ui/realtimechartwidget.h"

#include <QtCharts/QChart>
#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QTextStream>
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ===== 1. 加载 QSS 深色主题 =====
    {
        QFile file(QStringLiteral(":/style.qss"));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            qobject_cast<QApplication*>(QCoreApplication::instance())->setStyleSheet(stream.readAll());
        }
    }

    // ===== 2. 构建 UI（View 层）=====
    buildUi();

    // ===== 3. 创建 Controller，注入 View，启动 Worker =====
    m_controller = new MainController(this);
    m_controller->setViews(m_connPanel, m_chartWidget, m_tableWidget);
    m_controller->startWorkers();

    statusBar()->showMessage(QStringLiteral("就绪"));
    setWindowTitle(QStringLiteral("通用工业设备调试与监控上位机"));
    resize(1100, 700);
}

MainWindow::~MainWindow()
{
    // MainController 是 QObject 父子关系挂在 this 下，会自动析构
}

void MainWindow::buildUi()
{
    // ===== 中央：通信终端 =====
    m_connPanel = new ConnectionPanel(this);
    setCentralWidget(m_connPanel->terminalWidget());

    m_chartWidget = new RealtimeChartWidget(this);
    m_tableWidget = new DataTableWidget(this);

    // Dock 1: 连接面板（左上）
    auto *connDock = new QDockWidget(QStringLiteral("连接与配置"), this);
    connDock->setWidget(m_connPanel);
    connDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    connDock->setMinimumWidth(250);
    addDockWidget(Qt::LeftDockWidgetArea, connDock);

    // Dock 2: 字段选择（左下）
    auto *legendDock = new QDockWidget(QStringLiteral("字段选择"), this);
    legendDock->setWidget(m_chartWidget->legendWidget());
    legendDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    legendDock->setMinimumWidth(250);
    addDockWidget(Qt::LeftDockWidgetArea, legendDock);
    splitDockWidget(connDock, legendDock, Qt::Vertical);

    // Dock 3: 实时数据表格（右上）
    auto *tableDock = new QDockWidget(QStringLiteral("实时数据表格"), this);
    tableDock->setWidget(m_tableWidget);
    tableDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    tableDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, tableDock);

    // Dock 4: 实时数据曲线（右下）
    auto *chartDock = new QDockWidget(QStringLiteral("实时数据曲线"), this);
    chartDock->setWidget(m_chartWidget->chartWidget());
    chartDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    chartDock->setMinimumWidth(360);
    chartDock->setMinimumHeight(220);
    addDockWidget(Qt::RightDockWidgetArea, chartDock);
    splitDockWidget(tableDock, chartDock, Qt::Vertical);

    setDockNestingEnabled(true);

    // 启动后把左侧两个 Dock 压窄
    QTimer::singleShot(0, this, [this, connDock, legendDock]() {
        resizeDocks({connDock, legendDock}, {260, 260}, Qt::Horizontal);
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_controller) m_controller->stopWorkers();
    event->accept();
}
