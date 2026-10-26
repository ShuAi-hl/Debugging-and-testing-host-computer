#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainController;
class ConnectionPanel;
class RealtimeChartWidget;
class DataTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void buildUi();

    // Controller（唯一持有 Worker + 线程的对象）
    MainController *m_controller = nullptr;

    // UI 组件指针（Controller 需要连接）
    ConnectionPanel     *m_connPanel   = nullptr;
    RealtimeChartWidget *m_chartWidget = nullptr;
    DataTableWidget     *m_tableWidget = nullptr;
};

#endif // MAINWINDOW_H
