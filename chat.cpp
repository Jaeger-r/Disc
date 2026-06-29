#include "chat.h"

#include <QApplication>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include "./kernel/IKernel.h"

Chat::Chat(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("chatWindow"));
    buildUi();
    applyStyle();
    setupNotification();
    m_onlineRefreshTimer = new QTimer(this);
    m_onlineRefreshTimer->setInterval(3000);
    connect(m_onlineRefreshTimer, &QTimer::timeout, this, &Chat::requestOnlineUsers);
    setWindowTitle(QStringLiteral("聊天"));
    resize(520, 560);
}

Chat::~Chat() = default;

void Chat::setKernel(IKernel* kernel)
{
    m_kernel = kernel;
}

void Chat::setCurrentUser(long long userId, const QString& userName)
{
    m_currentUserId = userId;
    m_currentUserName = userName;
    requestOnlineUsers();
    if (m_currentUserId > 0 && !m_onlineRefreshTimer->isActive()) {
        m_onlineRefreshTimer->start();
    }
}

void Chat::appendIncomingMessage(STRU_CHAT_RS* response)
{
    if (!response) {
        return;
    }

    const QString userName = QString::fromLocal8Bit(response->m_userName).trimmed();
    const QString content = QString::fromLocal8Bit(response->szbuf).trimmed();
    m_historyEdit->append(QStringLiteral("<b>%1</b><br/>%2").arg(userName.toHtmlEscaped(),
                                                                 content.toHtmlEscaped()));
    if (!userName.isEmpty() && userName != m_currentUserName) {
        notifyIncomingMessage(userName, content);
    }
}

void Chat::updateOnlineUsers(STRU_ONLINE_USERS_RS* response)
{
    if (!response || response->m_userId != m_currentUserId || !m_userList) {
        return;
    }

    const long long previousPeerId = m_currentPeerId;
    m_onlineUsers.clear();
    m_userList->clear();

    for (int i = 0; i < response->m_userCount && i < ONLINEUSERNUM; ++i) {
        const long long userId = response->m_users[i].m_userId;
        const QString userName = QString::fromLocal8Bit(response->m_users[i].m_userName).trimmed();
        if (userId <= 0 || userName.isEmpty()) {
            continue;
        }
        m_onlineUsers.insert(userId, userName);
        auto* item = new QListWidgetItem(m_userList);
        item->setData(Qt::UserRole, QVariant::fromValue<qlonglong>(userId));
        item->setData(Qt::UserRole + 1, response->m_users[i].m_online != 0);
        const QString statusText = response->m_users[i].m_online ? QStringLiteral("在线") : QStringLiteral("离线");
        item->setText(m_unreadCounts.value(userId, 0) > 0
                          ? QStringLiteral("%1 [%2]  (%3)").arg(userName, statusText).arg(m_unreadCounts.value(userId))
                          : QStringLiteral("%1 [%2]").arg(userName, statusText));
        m_userList->addItem(item);
        if (userId == previousPeerId) {
            item->setSelected(true);
        }
    }

    if (previousPeerId > 0 && m_onlineUsers.contains(previousPeerId)) {
        m_currentPeerName = m_onlineUsers.value(previousPeerId);
    }
}

void Chat::appendPrivateMessage(STRU_PRIVATE_CHAT_RS* response)
{
    if (!response) {
        return;
    }

    const bool belongsToMe = response->m_senderId == m_currentUserId || response->m_receiverId == m_currentUserId;
    if (!belongsToMe) {
        return;
    }

    const long long peerId = response->m_senderId == m_currentUserId ? response->m_receiverId : response->m_senderId;
    const QString peerName = response->m_senderId == m_currentUserId
        ? QString::fromLocal8Bit(response->m_receiverName).trimmed()
        : QString::fromLocal8Bit(response->m_senderName).trimmed();
    if (peerId > 0 && !peerName.isEmpty() && !m_onlineUsers.contains(peerId)) {
        m_onlineUsers.insert(peerId, peerName);
    }

    const QString senderName = QString::fromLocal8Bit(response->m_senderName).trimmed();
    const QString content = QString::fromLocal8Bit(response->szbuf).trimmed();
    if (peerId == m_currentPeerId) {
        appendConversationLine(response->m_senderId, senderName, content);
    } else if (response->m_senderId != m_currentUserId) {
        setUnread(peerId, true);
    }

    if (response->m_senderId != m_currentUserId) {
        notifyIncomingMessage(senderName, content);
    }
}

void Chat::loadPrivateHistory(STRU_PRIVATE_HISTORY_RS* response)
{
    if (!response || response->m_userId != m_currentUserId || response->m_peerId != m_currentPeerId) {
        return;
    }

    m_historyEdit->clear();
    for (int i = 0; i < response->m_messageCount && i < CHATHISTORYNUM; ++i) {
        const ChatHistoryInfo& item = response->m_messages[i];
        appendConversationLine(item.m_senderId,
                               QString::fromLocal8Bit(item.m_senderName).trimmed(),
                               QString::fromLocal8Bit(item.szbuf).trimmed(),
                               QString::fromLocal8Bit(item.m_createdAt).trimmed());
    }
}

void Chat::requestOnlineUsers()
{
    if (!m_kernel || !m_kernel->isConnected() || m_currentUserId <= 0) {
        return;
    }

    STRU_ONLINE_USERS_RQ request;
    request.m_userId = m_currentUserId;
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Chat::sendMessage()
{
    if (!m_kernel || !m_kernel->isConnected()) {
        return;
    }
    if (m_currentUserId <= 0 || m_currentPeerId <= 0) {
        m_peerLabel->setText(QStringLiteral("请先选择左侧在线用户"));
        return;
    }

    const QString message = m_inputEdit->text().trimmed();
    if (message.isEmpty()) {
        return;
    }

    STRU_PRIVATE_CHAT_RQ request;
    request.m_senderId = m_currentUserId;
    request.m_receiverId = m_currentPeerId;
    qstrncpy(request.m_senderName, m_currentUserName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_receiverName, m_currentPeerName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.szbuf, message.toLocal8Bit().constData(), MAXSENDMESSSAGE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));

    appendConversationLine(m_currentUserId, m_currentUserName, message);
    m_inputEdit->clear();
}

void Chat::buildUi()
{
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(14);

    auto* sideLayout = new QVBoxLayout;
    auto* header = new QLabel(QStringLiteral("在线用户"), this);
    header->setObjectName(QStringLiteral("chatHeader"));
    sideLayout->addWidget(header);

    m_userList = new QListWidget(this);
    m_userList->setObjectName(QStringLiteral("onlineUserList"));
    connect(m_userList, &QListWidget::itemClicked, this, &Chat::onUserActivated);
    sideLayout->addWidget(m_userList, 1);
    rootLayout->addLayout(sideLayout, 0);

    auto* conversationLayout = new QVBoxLayout;
    conversationLayout->setSpacing(12);

    m_peerLabel = new QLabel(QStringLiteral("选择左侧在线用户开始私聊"), this);
    m_peerLabel->setObjectName(QStringLiteral("chatPeerLabel"));
    conversationLayout->addWidget(m_peerLabel);

    m_historyEdit = new QTextEdit(this);
    m_historyEdit->setReadOnly(true);
    conversationLayout->addWidget(m_historyEdit, 1);

    auto* inputLayout = new QHBoxLayout;
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText(QStringLiteral("输入私聊消息后按回车发送"));
    m_sendButton = new QPushButton(QStringLiteral("发送"), this);
    m_sendButton->setAutoDefault(true);
    connect(m_sendButton, &QPushButton::clicked, this, &Chat::sendMessage);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &Chat::sendMessage);
    inputLayout->addWidget(m_inputEdit, 1);
    inputLayout->addWidget(m_sendButton);
    conversationLayout->addLayout(inputLayout);
    rootLayout->addLayout(conversationLayout, 1);
}

void Chat::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QWidget#chatWindow { background: #f2f7fa; color: #16324f; }"
        "QLabel { background: transparent; }"
        "QLabel#chatHeader { font-size: 22px; font-weight: 700; }"
        "QLabel#chatPeerLabel { font-size: 18px; font-weight: 700; }"
        "QListWidget#onlineUserList { min-width: 190px; max-width: 240px; border: 1px solid #d0dce6; border-radius: 8px; background: white; padding: 6px; }"
        "QListWidget#onlineUserList::item { min-height: 34px; padding: 6px 8px; border-radius: 6px; }"
        "QListWidget#onlineUserList::item:selected { background: #d8edf2; color: #16324f; }"
        "QTextEdit { border: 1px solid #d0dce6; border-radius: 18px; padding: 12px; background: white; }"
        "QLineEdit { min-height: 40px; border-radius: 14px; padding: 0 12px; border: 1px solid #d0dce6; background: white; }"
        "QPushButton { min-width: 88px; min-height: 40px; border-radius: 14px; background: #1f7a8c; color: white; font-weight: 600; border: none; }"
        "QPushButton:hover { background: #165f6d; }"));
}

void Chat::onUserActivated(QListWidgetItem* item)
{
    if (!item) {
        return;
    }

    const long long peerId = item->data(Qt::UserRole).toLongLong();
    QString peerName = m_onlineUsers.value(peerId);
    if (peerName.isEmpty()) {
        peerName = item->text().section(QStringLiteral(" ["), 0, 0);
    }
    setCurrentPeer(peerId, peerName);
}

void Chat::requestHistory(long long peerId)
{
    if (!m_kernel || !m_kernel->isConnected() || m_currentUserId <= 0 || peerId <= 0) {
        return;
    }

    STRU_PRIVATE_HISTORY_RQ request;
    request.m_userId = m_currentUserId;
    request.m_peerId = peerId;
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Chat::setCurrentPeer(long long peerId, const QString& peerName)
{
    if (peerId <= 0) {
        return;
    }

    m_currentPeerId = peerId;
    m_currentPeerName = peerName;
    m_peerLabel->setText(QStringLiteral("正在与 %1 私聊").arg(peerName));
    m_historyEdit->clear();
    setUnread(peerId, false);
    requestHistory(peerId);
    m_inputEdit->setFocus();
}

void Chat::appendConversationLine(long long senderId, const QString& senderName, const QString& content, const QString& createdAt)
{
    const QString displayName = senderId == m_currentUserId
        ? QStringLiteral("%1（我）").arg(m_currentUserName)
        : senderName;
    const QString timePrefix = createdAt.isEmpty()
        ? QString()
        : QStringLiteral("<span style=\"color:#789; font-size:12px;\">%1</span><br/>").arg(createdAt.toHtmlEscaped());
    m_historyEdit->append(QStringLiteral("%1<b>%2</b><br/>%3")
                              .arg(timePrefix,
                                   displayName.toHtmlEscaped(),
                                   content.toHtmlEscaped()));
}

void Chat::setUnread(long long peerId, bool unread)
{
    if (peerId <= 0) {
        return;
    }

    if (unread) {
        m_unreadCounts[peerId] = m_unreadCounts.value(peerId, 0) + 1;
    } else {
        m_unreadCounts.remove(peerId);
    }

    if (!m_userList) {
        return;
    }

    for (int i = 0; i < m_userList->count(); ++i) {
        auto* item = m_userList->item(i);
        if (!item || item->data(Qt::UserRole).toLongLong() != peerId) {
            continue;
        }
        const QString userName = m_onlineUsers.value(peerId, item->text().section(QStringLiteral(" ["), 0, 0));
        const QString statusText = item->data(Qt::UserRole + 1).toBool() ? QStringLiteral("在线") : QStringLiteral("离线");
        const int unreadCount = m_unreadCounts.value(peerId, 0);
        item->setText(unreadCount > 0
                          ? QStringLiteral("%1 [%2]  (%3)").arg(userName, statusText).arg(unreadCount)
                          : QStringLiteral("%1 [%2]").arg(userName, statusText));
        QFont font = item->font();
        font.setBold(unreadCount > 0);
        item->setFont(font);
        break;
    }
}

void Chat::setupNotification()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    QIcon icon(QStringLiteral(":/app/DiskClientIcon.png"));
    if (icon.isNull()) {
        icon = windowIcon();
    }
    if (!icon.isNull()) {
        m_trayIcon->setIcon(icon);
    }
    m_trayIcon->setToolTip(QStringLiteral("DiskClient"));
    m_trayIcon->show();
}

void Chat::notifyIncomingMessage(const QString& userName, const QString& content)
{
    const QString title = QStringLiteral("来自 %1 的新消息").arg(userName);
    QString body = content.simplified();
    if (body.size() > 120) {
        body = body.left(117) + QStringLiteral("...");
    }

    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, body, QSystemTrayIcon::Information, 5000);
    }

    QApplication::alert(this, 0);
    QApplication::beep();
}
