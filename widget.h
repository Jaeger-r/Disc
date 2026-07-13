#ifndef WIDGET_H
#define WIDGET_H

#include <QDateTime>
#include <QHash>
#include <QPointer>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

#include "./kernel/tcpkernel.h"
#include "chat.h"
#include "login.h"
#include "transferpanel.h"

class QFile;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QTableWidget;
class QTextEdit;

struct TransferTask
{
    enum Direction {
        Upload,
        Download
    };

    enum State {
        Queued,
        AwaitingServer,
        Running,
        Paused,
        Completed,
        Failed,
        Cancelled
    };

    QString key;
    QString displayName;
    QString localPath;
    QString md5;
    qint64 fileId = 0;
    qint64 totalBytes = 0;
    qint64 transferredBytes = 0;
    qint64 acknowledgedBytes = 0;
    qint64 resumePos = 0;
    qint64 recentBytes = 0;
    qint64 bytesPerSecond = 0;
    Direction direction = Upload;
    State state = Queued;
    QString detailText;
    QPointer<QFile> file;
    QDateTime speedWindow;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget() override;

public slots:
    void slot_login(STRU_LOGIN_RS*);
    void slot_getfilelist(STRU_GETFILELIST_RS*);
    void slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
    void slot_uploadfileblock(STRU_UPLOADFILEBLOCK_RS*);
    void slot_deletefile(STRU_DELETEFILE_RS*);
    void slot_renamefile(STRU_RENAMEFILE_RS*);
    void slot_filesync(STRU_FILESYNC_RS*);
    void slot_downloadfileinfo(STRU_DOWNLOADFILEINFO_RS*);
    void slot_downloadfileblock(STRU_DOWNLOADFILEBLOCK_RS*);
    void slot_chat(STRU_CHAT_RS*);
    void slot_onlineusers(STRU_ONLINE_USERS_RS*);
    void slot_privatechat(STRU_PRIVATE_CHAT_RS*);
    void slot_privatehistory(STRU_PRIVATE_HISTORY_RS*);
    void slot_profileupdate(STRU_PROFILE_UPDATE_RS*);
    void slot_transfercontrol(STRU_TRANSFERCONTROL_RS*);
    void slot_connectionStateChanged(bool connected, const QString& reason);

private slots:
    void onUploadClicked();
    void onDownloadClicked();
    void onDeleteClicked();
    void onChatClicked();
    void onRefreshClicked();
    void onShareClicked();
    void onSelectionChanged();
    void onFileTableContextMenuRequested(const QPoint& pos);
    void onTransfersClicked();
    void onProfileSettingsClicked();
    void onSwitchAccountClicked();
    void onTransferPauseRequested(const QString& taskKey);
    void onTransferResumeRequested(const QString& taskKey);
    void onTransferCancelRequested(const QString& taskKey);
    void processUploadPump();
    void updateTransferRates();

private:
    void buildUi();
    void applyStyle();
    void appendStatus(const QString& message);
    void refreshFileList();
    void addOrUpdateFileRow(const QString& fileName, qint64 fileSize, const QString& uploadTime, const QString& md5);
    void addOrUpdateFileRow(const QString& fileName, qint64 fileSize, const QString& uploadTime, const QString& md5, int fileState);
    void applyFileStateRowStyle(int row, int fileState);
    void requestRenameSelectedFile();
    void requestResumeSelectedUpload();
    bool selectRowAtViewportPos(const QPoint& pos);
    QString selectedFileMd5() const;
    QString selectedFileName() const;
    qint64 selectedFileSize() const;
    int selectedFileState() const;
    QString formatSize(qint64 bytes) const;
    QString fileStateText(int fileState) const;
    QString computeFileMd5(const QString& filePath) const;
    QString localSettingsKey(const QString& key) const;
    QString localSettingsKeyForUser(qint64 userId, const QString& key) const;
    QString localAvatarPath(qint64 userId = -1) const;
    void saveLocalAvatarPath(const QString& path);
    QPixmap avatarPixmap(const QString& userName, int size, qint64 userId = -1) const;
    void openProfileSettings();
    void chooseProfileAvatar(QLabel* preview, QLabel* pathLabel);
    void sendProfileUpdate(QLineEdit* nameEdit, QLineEdit* passwordEdit);
    QStringList savedAccounts() const;
    qint64 savedAccountId(const QString& userName) const;
    void rememberAccount(const QString& userName, qint64 userId);
    void showLoginForAccount(const QString& userName);
    void switchToSavedAccount(const QString& userName);

    QString makeTaskKey(TransferTask::Direction direction, const QString& md5) const;
    TransferTaskSnapshot makeSnapshot(const TransferTask* task) const;
    void syncTransferPanelTask(TransferTask* task);
    void syncTransferSummary();
    void startUploadHandshake(TransferTask* task);
    void startDownloadHandshake(TransferTask* task, bool resumeExistingFile);
    bool openTaskFileForUpload(TransferTask* task, qint64 startPos);
    bool openTaskFileForDownload(TransferTask* task, bool resumeExistingFile);
    void closeTaskFile(TransferTask* task);
    void markTaskFailed(TransferTask* task, const QString& detailText);
    void markTaskCompleted(TransferTask* task, const QString& detailText);
    void markTaskCancelled(TransferTask* task, const QString& detailText);
    void markTaskPaused(TransferTask* task, const QString& detailText);
    void ensureUploadPump();
    void stopUploadPumpIfIdle();
    void sendTransferControl(TransferTask* task, char action);
    void clearTaskIfTerminal(TransferTask* task);
    TransferTask* findTaskByUploadResponse(const QString& fileName, const QString& md5 = QString()) const;
    TransferTask* findTaskByFileId(TransferTask::Direction direction, qint64 fileId) const;

    QLabel* m_connectionBadge = nullptr;
    QLabel* m_userLabel = nullptr;
    QPushButton* m_profileButton = nullptr;
    QTableWidget* m_fileTable = nullptr;
    QTextEdit* m_statusLog = nullptr;
    QPushButton* m_uploadButton = nullptr;
    QPushButton* m_downloadButton = nullptr;
    QPushButton* m_deleteButton = nullptr;
    QPushButton* m_chatButton = nullptr;
    QPushButton* m_refreshButton = nullptr;
    QPushButton* m_shareButton = nullptr;
    QPushButton* m_transfersButton = nullptr;

    Login* m_login = nullptr;
    Chat* m_chat = nullptr;
    TransferPanel* m_transferPanel = nullptr;
    IKernel* m_kernel = nullptr;
    qint64 m_userId = 0;
    QString m_currentUserName;
    QHash<QString, TransferTask*> m_transferTasks;
    QTimer m_uploadPumpTimer;
    QTimer m_transferRateTimer;
};

#endif // WIDGET_H
