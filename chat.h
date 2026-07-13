#ifndef CHAT_H
#define CHAT_H

#include <QHash>
#include <QPixmap>
#include <QWidget>

#include "Packdef.h"

class IKernel;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QFrame;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QGridLayout;
class QVBoxLayout;
class QSystemTrayIcon;
class QTimer;

class ConversationPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ConversationPanel(QWidget* parent = nullptr);
    void setBackground(const QString& color, const QString& imagePath);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_color = QStringLiteral("#edf4f6");
    QString m_imagePath;
    QPixmap m_backgroundPixmap;
};

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
    void handleProfileUpdate(STRU_PROFILE_UPDATE_RS* response);
    void requestOnlineUsers();
    void reloadLocalSettings();

private slots:
    void sendMessage();
    void onUserActivated(QListWidgetItem* item);
    void openSettings();
    void chooseBackgroundImage();
    void chooseBackgroundColor();
    void toggleAttachmentPanel();
    void toggleEmojiPanel();
    void handleAttachmentAction(const QString& action);
    void handleEmojiPicked(const QString& emoji);

private:
    void buildUi();
    void applyStyle();
    void loadLocalSettings();
    void saveLocalSettings();
    void applyConversationBackground();
    void applyBackgroundTheme(const QString& themeId);
    void rebuildMessages();
    void setupNotification();
    void notifyIncomingMessage(const QString& userName, const QString& content);
    void requestHistory(long long peerId);
    void setCurrentPeer(long long peerId, const QString& peerName);
    void appendConversationLine(long long senderId, const QString& senderName, const QString& content, const QString& createdAt = QString());
    void sendTextMessage(const QString& message);
    void addMessageBubble(long long senderId, const QString& senderName, const QString& content, const QString& createdAt);
    void addTimeSeparator(const QString& createdAt);
    void addSystemNotice(const QString& content);
    void showDanmakuEditor(const QString& senderName);
    void showDanmaku(const QString& text);
    void sendPatAction(const QString& targetName);
    void showPatEffects(const QString& text);
    void showEmojiBurst(const QString& emoji);
    bool isEmojiMessage(const QString& content) const;
    bool shouldShowTimeSeparator(const QString& createdAt) const;
    QPixmap avatarFor(long long userId, const QString& userName) const;
    QPixmap roundedAvatar(const QPixmap& source, int size) const;
    void setUnread(long long peerId, bool unread);

    IKernel* m_kernel = nullptr;
    long long m_currentUserId = 0;
    long long m_currentPeerId = 0;
    QString m_currentUserName;
    QString m_currentPeerName;
    QString m_avatarPath;
    QString m_backgroundImagePath;
    QString m_backgroundColor = QStringLiteral("#edf4f6");
    QString m_backgroundTheme = QStringLiteral("mist");
    QString m_patText = QStringLiteral("轻轻拍了拍，留下了一阵温柔的回声");
    QHash<long long, QString> m_onlineUsers;
    QHash<long long, int> m_unreadCounts;
    QString m_lastTimeSeparator;
    struct MessageEntry {
        long long senderId = 0;
        QString senderName;
        QString content;
        QString createdAt;
    };
    QList<MessageEntry> m_messages;
    QLabel* m_peerLabel = nullptr;
    QLabel* m_avatarPreview = nullptr;
    QLabel* m_avatarPathLabel = nullptr;
    QLabel* m_backgroundLabel = nullptr;
    QListWidget* m_userList = nullptr;
    ConversationPanel* m_conversationPanel = nullptr;
    QWidget* m_messageContainer = nullptr;
    QVBoxLayout* m_messageLayout = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QLineEdit* m_inputEdit = nullptr;
    QPushButton* m_sendButton = nullptr;
    QPushButton* m_moreButton = nullptr;
    QPushButton* m_emojiButton = nullptr;
    QPushButton* m_settingsButton = nullptr;
    QWidget* m_attachmentPanel = nullptr;
    QWidget* m_emojiPanel = nullptr;
    QWidget* m_settingsPanel = nullptr;
    QSystemTrayIcon* m_trayIcon = nullptr;
    QTimer* m_onlineRefreshTimer = nullptr;
};

#endif // CHAT_H
