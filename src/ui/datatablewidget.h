#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTimer>
#include "../core/devicedatapoint.h"
#include "../core/databus.h"

/**
 * @brief 数据表格组件（DockWidget 内容）
 *
 * 实时显示所有数据点的最新值、单位、来源、更新时间。
 * 支持按名称过滤、清空、CSV 导出。
 */
class DataTableWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataTableWidget(QWidget *parent = nullptr);
    ~DataTableWidget() override = default;

public slots:
    void onDataConsumed(const DeviceDataPoint &dp);

private slots:
    void onFilterChanged(const QString &text);
    void onClearClicked();
    void onExportClicked();

private:
    void buildUi();
    void upsertRow(const DeviceDataPoint &dp);

    QTableWidget *m_table = nullptr;
    QLineEdit    *m_filterEdit = nullptr;
    int          m_nameCol = 0;
    int          m_valueCol = 1;
    int          m_unitCol = 2;
    int          m_sourceCol = 3;
    int          m_timeCol = 4;
};

#endif // DATATABLEWIDGET_H
