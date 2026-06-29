#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QHash>

#include "Packdef.h"

class IKernel;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QSystemTrayIcon;
class QTimer;

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget* parent = nullptr);
    ~Chat() override;

    void setKernel(IKernel* kernel);
    void setCurrentUser(long long userId, const QString& userName);
    void appendIncomingMessage(STRU_CHAT_RS* response);
    void updateOnlineUsers(STRU_ONLINE_USERS_RS* response);
    void appendPrivateMessage(STRU_PRIVATE_CHAT_RS* response);
    void loadPrivateHistory(STRU_PRIVATE_HISTORY_RS* response);
    void requestOnlineUsers();

private slots:
    void sendMessage();
    void onUserActivated(QListWidgetItem* item);

private:
    void buildUi();
    void applyStyle();
    void setupNotification();
    void notifyIncomingMessage(const QString& userName, const QString& content);
    void requestHistory(long long peerId);
    void setCurrentPeer(long long peerId, const QString& peerName);
    void appendConversationLine(long long senderId, const QString& senderName, const QString& content, const QString& createdAt = QString());
    void setUnread(long long peerId, bool unread);

    IKernel* m_kernel = nullptr;
    long long m_currentUserId = 0;
    long long m_currentPeerId = 0;
    QString m_currentUserName;
    QString m_currentPeerName;
    QHash<long long, QString> m_onlineUsers;
    QHash<long long, int> m_unreadCounts;
    QLabel* m_peerLabel = nullptr;
    QListWidget* m_userList = nullptr;
    QTextEdit* m_historyEdit = nullptr;
    QLineEdit* m_inputEdit = nullptr;
    QPushButton* m_sendButton = nullptr;
    QSystemTrayIcon* m_trayIcon = nullptr;
    QTimer* m_onlineRefreshTimer = nullptr;
};

#endif // CHAT_H
