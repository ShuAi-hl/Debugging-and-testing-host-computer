#include "datatablewidget.h"
#include <QLabel>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

DataTableWidget::DataTableWidget(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
}

void DataTableWidget::buildUi()
{
    auto *mainLayout = new QVBoxLayout(this);

    // 顶部工具栏
    auto *toolRow = new QHBoxLayout();
    toolRow->addWidget(new QLabel(QStringLiteral("过滤:"), this));
    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText(QStringLiteral("输入关键字过滤..."));
    toolRow->addWidget(m_filterEdit, 1);

    auto *clearBtn = new QPushButton(QStringLiteral("清空"), this);
    auto *exportBtn = new QPushButton(QStringLiteral("导出CSV"), this);
    toolRow->addWidget(clearBtn);
    toolRow->addWidget(exportBtn);
    mainLayout->addLayout(toolRow);

    // 表格
    m_table = new QTableWidget(0, 5, this);
    QStringList headers = {QStringLiteral("名称"), QStringLiteral("值"),
                           QStringLiteral("单位"), QStringLiteral("来源"),
                           QStringLiteral("更新时间")};
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    mainLayout->addWidget(m_table, 1);

    // 连接信号
    connect(m_filterEdit, &QLineEdit::textChanged, this, &DataTableWidget::onFilterChanged);
    connect(clearBtn, &QPushButton::clicked, this, &DataTableWidget::onClearClicked);
    connect(exportBtn, &QPushButton::clicked, this, &DataTableWidget::onExportClicked);
}

void DataTableWidget::onDataConsumed(const DeviceDataPoint &dp)
{
    upsertRow(dp);
}

void DataTableWidget::upsertRow(const DeviceDataPoint &dp)
{
    // 查找已有行
    int row = -1;
    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (m_table->item(r, m_nameCol) &&
            m_table->item(r, m_nameCol)->text() == dp.name) {
            row = r;
            break;
        }
    }
    if (row < 0) {
        row = m_table->rowCount();
        m_table->insertRow(row);
        for (int c = 0; c < 5; ++c) {
            m_table->setItem(row, c, new QTableWidgetItem());
        }
    }

    m_table->item(row, m_nameCol)->setText(dp.name);
    m_table->item(row, m_valueCol)->setText(QStringLiteral("%1").arg(dp.value, 0, 'f', 3));
    m_table->item(row, m_unitCol)->setText(dp.unit);
    m_table->item(row, m_sourceCol)->setText(dp.source);
    m_table->item(row, m_timeCol)->setText(
        dp.timestamp.toString(QStringLiteral("HH:mm:ss.zzz")));

    // 如果有过滤器，应用可见性
    if (!m_filterEdit->text().isEmpty()) {
        bool match = dp.name.contains(m_filterEdit->text(), Qt::CaseInsensitive);
        m_table->setRowHidden(row, !match);
    } else {
        m_table->setRowHidden(row, false);
    }
}

void DataTableWidget::onFilterChanged(const QString &text)
{
    for (int r = 0; r < m_table->rowCount(); ++r) {
        bool match = text.isEmpty() ||
                     (m_table->item(r, m_nameCol) &&
                      m_table->item(r, m_nameCol)->text().contains(text, Qt::CaseInsensitive));
        m_table->setRowHidden(r, !match);
    }
}

void DataTableWidget::onClearClicked()
{
    m_table->setRowCount(0);
}

void DataTableWidget::onExportClicked()
{
    QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出 CSV"),
        QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"))
            + QStringLiteral(".csv"),
        QStringLiteral("CSV Files (*.csv)"));
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream stream(&file);
    stream << "name,value,unit,source,timestamp\n";
    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (m_table->isRowHidden(r)) continue;
        QStringList row;
        for (int c = 0; c < 5; ++c) {
            auto *item = m_table->item(r, c);
            row << (item ? item->text() : QString());
        }
        stream << row.join(',') << '\n';
    }
    file.close();
}
