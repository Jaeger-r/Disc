#include "transferpanel.h"

#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

TransferPanel::TransferPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("transferPanelWindow"));
    setWindowTitle(QStringLiteral("传输管理"));
    resize(900, 460);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(12);

    m_summaryLabel = new QLabel(QStringLiteral("当前没有传输任务"), this);
    rootLayout->addWidget(m_summaryLabel);

    m_table = new QTableWidget(0, ColumnCount, this);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("方向"),
        QStringLiteral("文件"),
        QStringLiteral("进度"),
        QStringLiteral("速度"),
        QStringLiteral("状态"),
        QStringLiteral("说明")
    });
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnDirection, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnFileName, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnProgress, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnSpeed, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnState, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(ColumnDetail, QHeaderView::Stretch);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &TransferPanel::rebuildButtons);
    rootLayout->addWidget(m_table, 1);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    m_pauseButton = new QPushButton(QStringLiteral("暂停"), this);
    m_resumeButton = new QPushButton(QStringLiteral("继续"), this);
    m_cancelButton = new QPushButton(QStringLiteral("取消"), this);
    connect(m_pauseButton, &QPushButton::clicked, this, &TransferPanel::onPauseClicked);
    connect(m_resumeButton, &QPushButton::clicked, this, &TransferPanel::onResumeClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &TransferPanel::onCancelClicked);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_resumeButton);
    buttonLayout->addWidget(m_cancelButton);
    rootLayout->addLayout(buttonLayout);

    setStyleSheet(QStringLiteral(
        "QWidget#transferPanelWindow { background: #f3f7fb; color: #16324f; }"
        "QLabel { background: transparent; }"
        "QTableWidget { background: white; border-radius: 16px; border: 1px solid #d8e2eb; }"
        "QHeaderView::section { background: #dbe7f0; border: none; padding: 8px; font-weight: 700; }"
        "QPushButton { min-width: 84px; min-height: 36px; border: none; border-radius: 12px; background: #16324f; color: white; }"
        "QPushButton:hover { background: #244d73; }"
        "QPushButton:disabled { background: #b8c8d8; color: #5f7385; }"));

    rebuildButtons();
}

void TransferPanel::upsertTask(const TransferTaskSnapshot& snapshot)
{
    const int row = ensureRow(snapshot.key);
    const auto setColumnText = [this, row](int column, const QString& text) {
        auto* item = m_table->item(row, column);
        if (!item) {
            item = new QTableWidgetItem;
            m_table->setItem(row, column, item);
        }
        item->setText(text);
    };

    setColumnText(ColumnDirection, directionText(snapshot.direction));
    setColumnText(ColumnFileName, snapshot.displayName);
    setColumnText(ColumnProgress, formatProgress(snapshot.transferredBytes, snapshot.totalBytes));
    setColumnText(ColumnSpeed, snapshot.bytesPerSecond > 0
        ? QStringLiteral("%1/s").arg(formatBytes(snapshot.bytesPerSecond))
        : QStringLiteral("-"));
    setColumnText(ColumnState, stateText(snapshot.state));
    setColumnText(ColumnDetail, snapshot.detailText);

    m_table->item(row, ColumnFileName)->setData(Qt::UserRole, snapshot.key);
    rebuildButtons();
}

void TransferPanel::removeTask(const QString& taskKey)
{
    if (!m_rowByTaskKey.contains(taskKey)) {
        return;
    }

    const int row = m_rowByTaskKey.take(taskKey);
    m_table->removeRow(row);

    for (auto it = m_rowByTaskKey.begin(); it != m_rowByTaskKey.end(); ++it) {
        if (it.value() > row) {
            it.value() -= 1;
        }
    }

    rebuildButtons();
}

void TransferPanel::setSummaryText(const QString& text)
{
    m_summaryLabel->setText(text);
}

void TransferPanel::onPauseClicked()
{
    const QString taskKey = taskKeyForSender();
    if (!taskKey.isEmpty()) {
        emit pauseRequested(taskKey);
    }
}

void TransferPanel::onResumeClicked()
{
    const QString taskKey = taskKeyForSender();
    if (!taskKey.isEmpty()) {
        emit resumeRequested(taskKey);
    }
}

void TransferPanel::onCancelClicked()
{
    const QString taskKey = taskKeyForSender();
    if (!taskKey.isEmpty()) {
        emit cancelRequested(taskKey);
    }
}

QString TransferPanel::formatBytes(qint64 bytes) const
{
    if (bytes < 1024) {
        return QStringLiteral("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(QString::number(bytes / 1024.0, 'f', 1));
    }
    if (bytes < 1024ll * 1024ll * 1024ll) {
        return QStringLiteral("%1 MB").arg(QString::number(bytes / 1024.0 / 1024.0, 'f', 1));
    }
    return QStringLiteral("%1 GB").arg(QString::number(bytes / 1024.0 / 1024.0 / 1024.0, 'f', 1));
}

QString TransferPanel::formatProgress(qint64 transferred, qint64 total) const
{
    if (total <= 0) {
        return formatBytes(transferred);
    }

    const double percent = static_cast<double>(transferred) * 100.0 / static_cast<double>(total);
    return QStringLiteral("%1 / %2 (%3%)")
        .arg(formatBytes(transferred))
        .arg(formatBytes(total))
        .arg(QString::number(percent, 'f', percent < 10.0 ? 1 : 0));
}

QString TransferPanel::stateText(TransferTaskSnapshot::State state) const
{
    switch (state) {
    case TransferTaskSnapshot::Queued: return QStringLiteral("排队中");
    case TransferTaskSnapshot::Running: return QStringLiteral("传输中");
    case TransferTaskSnapshot::Paused: return QStringLiteral("已暂停");
    case TransferTaskSnapshot::Completed: return QStringLiteral("已完成");
    case TransferTaskSnapshot::Failed: return QStringLiteral("失败");
    case TransferTaskSnapshot::Cancelled: return QStringLiteral("已取消");
    }
    return QStringLiteral("未知");
}

QString TransferPanel::directionText(TransferTaskSnapshot::Direction direction) const
{
    return direction == TransferTaskSnapshot::Upload ? QStringLiteral("上传") : QStringLiteral("下载");
}

int TransferPanel::ensureRow(const QString& taskKey)
{
    if (m_rowByTaskKey.contains(taskKey)) {
        return m_rowByTaskKey.value(taskKey);
    }

    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_rowByTaskKey.insert(taskKey, row);
    return row;
}

QString TransferPanel::taskKeyForSender() const
{
    const auto items = m_table->selectedItems();
    if (items.isEmpty()) {
        return {};
    }

    const int row = items.first()->row();
    auto* item = m_table->item(row, ColumnFileName);
    return item ? item->data(Qt::UserRole).toString() : QString();
}

void TransferPanel::rebuildButtons()
{
    const bool hasSelection = !taskKeyForSender().isEmpty();
    m_pauseButton->setEnabled(hasSelection);
    m_resumeButton->setEnabled(hasSelection);
    m_cancelButton->setEnabled(hasSelection);
}
