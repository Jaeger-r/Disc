#ifndef TRANSFERPANEL_H
#define TRANSFERPANEL_H

#include <QObject>
#include <QHash>
#include <QWidget>

class QLabel;
class QPushButton;
class QTableWidget;

struct TransferTaskSnapshot
{
    enum Direction {
        Upload,
        Download
    };

    enum State {
        Queued,
        Running,
        Paused,
        Completed,
        Failed,
        Cancelled
    };

    QString key;
    QString displayName;
    QString md5;
    qint64 fileId = 0;
    qint64 totalBytes = 0;
    qint64 transferredBytes = 0;
    qint64 bytesPerSecond = 0;
    Direction direction = Upload;
    State state = Queued;
    QString detailText;
};

class TransferPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TransferPanel(QWidget* parent = nullptr);
    ~TransferPanel() override = default;

    void upsertTask(const TransferTaskSnapshot& snapshot);
    void removeTask(const QString& taskKey);
    void setSummaryText(const QString& text);

signals:
    void pauseRequested(const QString& taskKey);
    void resumeRequested(const QString& taskKey);
    void cancelRequested(const QString& taskKey);

private slots:
    void onPauseClicked();
    void onResumeClicked();
    void onCancelClicked();

private:
    enum Column {
        ColumnDirection = 0,
        ColumnFileName,
        ColumnProgress,
        ColumnSpeed,
        ColumnState,
        ColumnDetail,
        ColumnCount
    };

    QString formatBytes(qint64 bytes) const;
    QString formatProgress(qint64 transferred, qint64 total) const;
    QString stateText(TransferTaskSnapshot::State state) const;
    QString directionText(TransferTaskSnapshot::Direction direction) const;
    int ensureRow(const QString& taskKey);
    QString taskKeyForSender() const;
    void rebuildButtons();

    QLabel* m_summaryLabel = nullptr;
    QTableWidget* m_table = nullptr;
    QPushButton* m_pauseButton = nullptr;
    QPushButton* m_resumeButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    QHash<QString, int> m_rowByTaskKey;
};

#endif // TRANSFERPANEL_H
