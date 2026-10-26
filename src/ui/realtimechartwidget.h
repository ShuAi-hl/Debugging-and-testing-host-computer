#ifndef REALTIMECHARTWIDGET_H
#define REALTIMECHARTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QMap>
#include <QList>
#include <QColor>
#include <QPainter>
#include <QScrollArea>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "../core/devicedatapoint.h"
#include "../core/databus.h"

/**
 * @brief 实时曲线图表组件
 *
 * 特性：
 *   1) 自动跟踪 DataBus 中的所有数据点，为每个字段创建一条曲线
 *   2) 内置固定数据点上限（默认 500），防止长时间运行内存膨胀
 *   3) 支持勾选显示/隐藏曲线
 *   4) 支持时间窗口滚动视图或最新 N 点视图
 *
 * 线程安全：onDataConsumed 通常在 UI 线程被调用（因为 DataBus 的信号会被 UI 线程接收），
 *          如果未来需要从工作线程推送，需要把数据复制再排队。
 */
class RealtimeChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit RealtimeChartWidget(QWidget *parent = nullptr);
    ~RealtimeChartWidget() override = default;

    /// 设置单个系列的最大点数上限（默认 500）
    void setMaxPointsPerSeries(int max) { m_maxPoints = max; }

    /// 启用/禁用实时自动刷新（图表在数据量大时可能需要暂时冻结）
    void setAutoRefresh(bool on);

    /// 左侧图例/字段选择容器（由 MainWindow 取出独立摆放）
    QWidget *legendWidget() const { return m_legendContainer; }
    /// 图表视图容器（由 MainWindow 取出独立摆放）
    QWidget *chartWidget() const { return m_chartView; }

public slots:
    /// 处理单条数据点（UI 线程调用）
    void onDataConsumed(const DeviceDataPoint &dp);

    /// 批量处理（效率更高）
    void onBatchConsumed(const QList<DeviceDataPoint> &list);

private slots:
    void onSeriesVisibilityChanged(int state);
    void onWindowModeChanged(int idx);
    void refreshAxisRange();

private:
    void buildUi();
    void ensureSeriesFor(const QString &name);
    void appendPoint(const QString &name, double x, double y);
    void pruneSeries(QLineSeries *series);

    // 图表相关
    QChart *m_chart = nullptr;
    QChartView *m_chartView = nullptr;
    QValueAxis *m_axisX = nullptr;
    QValueAxis *m_axisY = nullptr;

    QStringList m_allFields;                // 所有已注册的字段（按注册顺序）
    QMap<QString, QLineSeries*> m_seriesMap;
    QMap<QString, QCheckBox*> m_checkMap;
    QMap<QString, int> m_xCounters;         // 每个字段独立的 X 计数器

    QVBoxLayout *m_legendLayout = nullptr;  // 图例复选框容器
    QWidget *m_legendContainer = nullptr;   // 图例整体容器（X轴模式 + 字段选择）

    int m_maxPoints = 500;                  // 每个系列的最大点数
    bool m_autoRefresh = true;

    int m_windowMode = 0;                   // 0=最新N点, 1=时间窗口(秒)
    int m_timeWindowSec = 60;

    QTimer *m_refreshTimer = nullptr;

    // 颜色表（调色板循环使用）
    static const QList<QColor> s_colors;
};

#endif // REALTIMECHARTWIDGET_H
