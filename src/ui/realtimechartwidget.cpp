#include "realtimechartwidget.h"
#include <QGridLayout>

const QList<QColor> RealtimeChartWidget::s_colors = {
    QColor(0xFF, 0x70, 0x43),  // 红
    QColor(0x46, 0xAD, 0xFF),  // 蓝
    QColor(0x5E, 0xC6, 0x33),  // 绿
    QColor(0xFF, 0xA5, 0x00),  // 橙
    QColor(0x9B, 0x59, 0xB6),  // 紫
    QColor(0x1A, 0xBC, 0x9C),  // 青
    QColor(0xE6, 0x7E, 0x22),  // 深橙
    QColor(0x2C, 0x3E, 0x50),  // 深灰
    QColor(0xE8, 0x43, 0x93),  // 粉
    QColor(0x34, 0x49, 0x5E),  // 深蓝灰
};

RealtimeChartWidget::RealtimeChartWidget(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

void RealtimeChartWidget::buildUi()
{
    // ========= 图例/字段选择容器（独立组件，由 MainWindow 摆放） =========
    m_legendContainer = new QWidget(this);
    auto *legendOuter = new QVBoxLayout(m_legendContainer);
    legendOuter->setContentsMargins(0, 0, 0, 0);

    auto *legendBox = new QGroupBox(QStringLiteral("字段选择"), m_legendContainer);
    m_legendLayout = new QVBoxLayout(legendBox);
    auto *placeholder = new QLabel(QStringLiteral("等待数据..."), m_legendContainer);
    placeholder->setObjectName(QStringLiteral("legendPlaceholder"));
    m_legendLayout->addWidget(placeholder);
    m_legendLayout->addStretch(1);

    auto *legendScroll = new QScrollArea(m_legendContainer);
    legendScroll->setWidgetResizable(true);
    legendScroll->setWidget(legendBox);

    auto *controlRow = new QHBoxLayout();
    controlRow->addWidget(new QLabel(QStringLiteral("X轴模式:")));
    auto *modeCombo = new QComboBox(m_legendContainer);
    modeCombo->addItem(QStringLiteral("最新 500 点"));
    modeCombo->addItem(QStringLiteral("时间窗口 60s"));
    connect(modeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &RealtimeChartWidget::onWindowModeChanged);
    controlRow->addWidget(modeCombo);
    controlRow->addStretch();

    legendOuter->addLayout(controlRow);
    legendOuter->addWidget(legendScroll, 1);

    // ========= 右侧：图表 =========
    m_chart = new QChart();
    m_chart->setTitle(QStringLiteral("实时数据曲线"));
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setPlotAreaBackgroundVisible(false);
    m_chart->setBackgroundVisible(false);

    // 所有文字白色
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    m_chart->setTitleFont(titleFont);
    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->legend()->setLabelColor(Qt::white);

    m_axisX = new QValueAxis();
    m_axisX->setTitleText(QStringLiteral("样本点"));
    m_axisX->setTitleBrush(QBrush(Qt::white));
    m_axisX->setLabelsColor(Qt::white);
    m_axisX->setLinePenColor(Qt::white);
    m_axisX->setGridLineColor(QColor(255, 255, 255, 40));
    m_axisX->setLabelFormat("%i");
    m_axisX->setRange(0, m_maxPoints);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText(QStringLiteral("数值"));
    m_axisY->setTitleBrush(QBrush(Qt::white));
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setLinePenColor(Qt::white);
    m_axisY->setGridLineColor(QColor(255, 255, 255, 40));
    m_axisY->setRange(0, 100);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    // 注意：m_chartView 和 m_legendContainer 都不加入本组件的布局，
    // 由 MainWindow 分别取出放到不同的 Dock 区域

    // ========= 刷新定时器 =========
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(1000);
    connect(m_refreshTimer, &QTimer::timeout, this, &RealtimeChartWidget::refreshAxisRange);
    m_refreshTimer->start();
}

void RealtimeChartWidget::ensureSeriesFor(const QString &name)
{
    if (m_seriesMap.contains(name)) return;

    auto *series = new QLineSeries();
    series->setName(name);

    int colorIdx = m_allFields.size() % s_colors.size();
    QPen pen(s_colors[colorIdx]);
    pen.setWidthF(2.0);
    series->setPen(pen);

    m_chart->addSeries(series);
    series->attachAxis(m_axisX);
    series->attachAxis(m_axisY);

    m_seriesMap[name] = series;
    m_allFields.append(name);

    // 在图例侧增加一个复选框 —— 第一次添加时移除占位提示
    if (m_allFields.size() == 1 && m_legendLayout) {
        // 移除 "等待数据..." 的占位 label
        QLayoutItem *item;
        while ((item = m_legendLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
    }

    auto *cb = new QCheckBox(name, this);
    cb->setChecked(true);
    cb->setStyleSheet(
        QStringLiteral("QCheckBox { color: %1; font-weight: bold; padding: 2px 0; }")
            .arg(s_colors[colorIdx].name()));

    // Qt 6: checkStateChanged 是 stateChanged 的推荐替代
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(cb, &QCheckBox::checkStateChanged, this,
#else
    connect(cb, &QCheckBox::stateChanged, this,
#endif
            [this, name](int state) {
                if (m_seriesMap.contains(name)) {
                    m_seriesMap[name]->setVisible(state == Qt::Checked);
                }
            });

    m_checkMap[name] = cb;
    if (m_legendLayout) {
        // 插入到倒数第二个位置（stretch 之前）
        m_legendLayout->insertWidget(m_legendLayout->count() - 1, cb);
    }
}

void RealtimeChartWidget::appendPoint(const QString &name, double x, double y)
{
    if (!m_seriesMap.contains(name)) return;
    auto *series = m_seriesMap[name];
    series->append(x, y);
    pruneSeries(series);
}

void RealtimeChartWidget::pruneSeries(QLineSeries *series)
{
    if (series->count() > m_maxPoints) {
        int toRemove = series->count() - m_maxPoints;
        for (int i = 0; i < toRemove; ++i) {
            series->remove(0);
        }
    }
}

void RealtimeChartWidget::onDataConsumed(const DeviceDataPoint &dp)
{
    if (!m_autoRefresh) return;
    ensureSeriesFor(dp.name);

    int x = m_xCounters.value(dp.name, 0) + 1;
    m_xCounters[dp.name] = x;

    appendPoint(dp.name, x, dp.value);
}

void RealtimeChartWidget::onBatchConsumed(const QList<DeviceDataPoint> &list)
{
    if (!m_autoRefresh) return;
    for (const auto &dp : list) {
        onDataConsumed(dp);
    }
}

void RealtimeChartWidget::onSeriesVisibilityChanged(int state)
{
    Q_UNUSED(state);
}

void RealtimeChartWidget::onWindowModeChanged(int idx)
{
    m_windowMode = idx;
    m_axisX->setTitleText(m_windowMode == 0
                              ? QStringLiteral("样本点")
                              : QStringLiteral("时间 (秒)"));
}

void RealtimeChartWidget::refreshAxisRange()
{
    if (!m_autoRefresh) return;

    double xMax = 0;
    double yMin = 1e9, yMax = -1e9;
    bool hasAny = false;

    for (auto it = m_seriesMap.begin(); it != m_seriesMap.end(); ++it) {
        auto *series = it.value();
        if (!series->isVisible() || series->count() == 0) continue;

        for (const auto &pt : series->points()) {
            if (pt.x() > xMax) xMax = pt.x();
            if (pt.y() < yMin) yMin = pt.y();
            if (pt.y() > yMax) yMax = pt.y();
            hasAny = true;
        }
    }

    if (!hasAny) return;

    double xMin = qMax(0.0, xMax - m_maxPoints);
    m_axisX->setRange(xMin, qMax(xMax, (double)m_maxPoints));

    double yRange = yMax - yMin;
    if (yRange < 1e-6) yRange = 1.0;
    double pad = yRange * 0.1;
    m_axisY->setRange(yMin - pad, yMax + pad);
}

void RealtimeChartWidget::setAutoRefresh(bool on)
{
    m_autoRefresh = on;
    if (on) m_refreshTimer->start();
    else    m_refreshTimer->stop();
}
