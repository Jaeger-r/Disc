#include "chat.h"

#include <QApplication>
#include <QColorDialog>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

#include "imagecropdialog.h"
#include "./kernel/IKernel.h"

namespace {
QString settingsKey(long long userId, const QString& key)
{
    return QStringLiteral("chat/%1/%2").arg(userId).arg(key);
}

QString initials(const QString& name)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        return QStringLiteral("U");
    }
    return trimmed.left(1).toUpper();
}

QIcon makeGearIcon(const QColor& color = QColor(22, 50, 79))
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(32, 32);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    for (int i = 0; i < 8; ++i) {
        painter.save();
        painter.rotate(i * 45.0);
        painter.drawRoundedRect(QRectF(-4, -29, 8, 14), 3, 3);
        painter.restore();
    }
    painter.drawEllipse(QPointF(0, 0), 20, 20);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(0, 0), 8, 8);
    return QIcon(pixmap);
}

QIcon makePlusIcon(const QColor& color = QColor(22, 50, 79))
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color, 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(32, 18), QPointF(32, 46));
    painter.drawLine(QPointF(18, 32), QPointF(46, 32));
    return QIcon(pixmap);
}

QIcon makeEmojiIcon(const QColor& color = QColor(22, 50, 79))
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(QRectF(14, 14, 36, 36));
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(27, 29), 2.7, 2.7);
    painter.drawEllipse(QPointF(39, 29), 2.7, 2.7);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(QRectF(25, 28, 16, 14), 205 * 16, 130 * 16);
    return QIcon(pixmap);
}

QIcon makeAttachmentIcon(const QString& kind)
{
    QPixmap pixmap(72, 72);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const QColor bg(233, 241, 246);
    const QColor ink(22, 50, 79);
    painter.setPen(Qt::NoPen);
    painter.setBrush(bg);
    painter.drawRoundedRect(QRectF(6, 6, 60, 60), 18, 18);
    painter.setPen(QPen(ink, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    if (kind == QStringLiteral("file")) {
        painter.drawRoundedRect(QRectF(23, 17, 26, 38), 4, 4);
        painter.drawLine(QPointF(30, 29), QPointF(43, 29));
        painter.drawLine(QPointF(30, 38), QPointF(43, 38));
    } else if (kind == QStringLiteral("cloud")) {
        QPainterPath path;
        path.moveTo(22, 46);
        path.cubicTo(14, 44, 14, 31, 25, 30);
        path.cubicTo(28, 20, 43, 19, 47, 30);
        path.cubicTo(58, 30, 60, 46, 48, 47);
        path.closeSubpath();
        painter.drawPath(path);
    } else if (kind == QStringLiteral("camera")) {
        painter.drawRoundedRect(QRectF(18, 25, 36, 26), 6, 6);
        painter.drawRoundedRect(QRectF(26, 19, 14, 8), 3, 3);
        painter.drawEllipse(QPointF(36, 38), 7, 7);
    } else if (kind == QStringLiteral("image")) {
        painter.drawRoundedRect(QRectF(18, 20, 36, 32), 5, 5);
        painter.drawEllipse(QPointF(30, 31), 3, 3);
        QPainterPath hill;
        hill.moveTo(22, 47);
        hill.lineTo(32, 38);
        hill.lineTo(39, 44);
        hill.lineTo(46, 34);
        hill.lineTo(52, 47);
        painter.drawPath(hill);
    } else {
        painter.drawRoundedRect(QRectF(17, 24, 32, 24), 5, 5);
        QPainterPath play;
        play.moveTo(31, 30);
        play.lineTo(43, 36);
        play.lineTo(31, 42);
        play.closeSubpath();
        painter.setBrush(ink);
        painter.drawPath(play);
    }

    return QIcon(pixmap);
}

QString cssColorWithAlpha(const QString& color, int alpha)
{
    const QColor parsed(color);
    const QColor safe = parsed.isValid() ? parsed : QColor(245, 249, 252);
    return QStringLiteral("rgba(%1,%2,%3,%4)")
        .arg(safe.red())
        .arg(safe.green())
        .arg(safe.blue())
        .arg(alpha / 255.0, 0, 'f', 2);
}

QStringList emojiChoices()
{
    return {
        QString::fromUtf8(u8"😀"),
        QString::fromUtf8(u8"😂"),
        QString::fromUtf8(u8"🥰"),
        QString::fromUtf8(u8"😎"),
        QString::fromUtf8(u8"👍"),
        QString::fromUtf8(u8"🎉"),
        QString::fromUtf8(u8"✨"),
        QString::fromUtf8(u8"❤️"),
        QString::fromUtf8(u8"🔥"),
        QString::fromUtf8(u8"🌙")
    };
}

QString patMessagePrefix()
{
    return QStringLiteral("[拍一拍]");
}

QString patDisplayText(const QString& content)
{
    return content.startsWith(patMessagePrefix())
        ? content.mid(patMessagePrefix().size()).trimmed()
        : content.trimmed();
}

struct ChatTheme {
    QString id;
    QString name;
    QString background;
    QString mineBubble;
    QString peerBubble;
    QString mineMeta;
    QString peerMeta;
    QString mineText;
    QString peerText;
};

QList<ChatTheme> chatThemes()
{
    return {
        {QStringLiteral("mist"), QStringLiteral("雾蓝"), QStringLiteral("#edf4f6"),
         QStringLiteral("rgba(104, 128, 151, 0.56)"), QStringLiteral("rgba(255,255,255,0.58)"),
         QStringLiteral("rgba(255,255,255,0.76)"), QStringLiteral("rgba(22,50,79,0.58)"),
         QStringLiteral("rgba(255,255,255,0.96)"), QStringLiteral("#16324f")},
        {QStringLiteral("linen"), QStringLiteral("亚麻"), QStringLiteral("#f0eee8"),
         QStringLiteral("rgba(132, 122, 104, 0.54)"), QStringLiteral("rgba(255,255,255,0.56)"),
         QStringLiteral("rgba(255,255,255,0.78)"), QStringLiteral("rgba(65,56,45,0.58)"),
         QStringLiteral("rgba(255,255,255,0.96)"), QStringLiteral("#2f3942")},
        {QStringLiteral("sage"), QStringLiteral("鼠尾草"), QStringLiteral("#e8efeb"),
         QStringLiteral("rgba(92, 125, 116, 0.55)"), QStringLiteral("rgba(255,255,255,0.57)"),
         QStringLiteral("rgba(255,255,255,0.78)"), QStringLiteral("rgba(31,63,57,0.58)"),
         QStringLiteral("rgba(255,255,255,0.96)"), QStringLiteral("#173a35")},
        {QStringLiteral("pearl"), QStringLiteral("珍珠灰"), QStringLiteral("#eceff2"),
         QStringLiteral("rgba(92, 105, 122, 0.56)"), QStringLiteral("rgba(255,255,255,0.58)"),
         QStringLiteral("rgba(255,255,255,0.78)"), QStringLiteral("rgba(35,48,64,0.58)"),
         QStringLiteral("rgba(255,255,255,0.96)"), QStringLiteral("#1b2f46")}
    };
}

ChatTheme chatThemeById(const QString& id)
{
    const QList<ChatTheme> themes = chatThemes();
    for (const ChatTheme& theme : themes) {
        if (theme.id == id) {
            return theme;
        }
    }
    return themes.first();
}

ChatTheme bubbleThemeForId(const QString& id)
{
    return id == QStringLiteral("custom") ? chatThemeById(QStringLiteral("mist")) : chatThemeById(id);
}

QString saveCroppedImage(const QPixmap& pixmap, const QString& prefix)
{
    if (pixmap.isNull()) {
        return QString();
    }
    const QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(baseDir.isEmpty() ? QDir::tempPath() : baseDir);
    dir.mkpath(QStringLiteral("images"));
    const QString filePath = dir.filePath(QStringLiteral("images/%1_%2.png")
        .arg(prefix, QString::number(QDateTime::currentMSecsSinceEpoch())));
    return pixmap.save(filePath, "PNG") ? filePath : QString();
}

QString protocolText(const char* value)
{
    return QString::fromUtf8(value ? value : "").trimmed();
}

void copyProtocolText(char* dest, qsizetype destSize, const QString& value)
{
    if (!dest || destSize <= 0) {
        return;
    }
    const QByteArray bytes = value.toUtf8();
    qstrncpy(dest, bytes.constData(), destSize);
}
}

ConversationPanel::ConversationPanel(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("conversationPanel"));
    setAttribute(Qt::WA_StyledBackground, false);
}

void ConversationPanel::setBackground(const QString& color, const QString& imagePath)
{
    m_color = color;
    m_imagePath = imagePath;
    m_backgroundPixmap = QPixmap();
    if (!m_imagePath.isEmpty() && QFileInfo::exists(m_imagePath)) {
        m_backgroundPixmap.load(m_imagePath);
    }
    update();
}

void ConversationPanel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath clip;
    clip.addRoundedRect(rect().adjusted(1, 1, -1, -1), 18, 18);
    painter.setClipPath(clip);
    painter.fillRect(rect(), QColor(m_color));

    if (!m_backgroundPixmap.isNull()) {
        const QSize targetSize = rect().size();
        const QPixmap scaled = m_backgroundPixmap.scaled(targetSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const int x = (targetSize.width() - scaled.width()) / 2;
        const int y = (targetSize.height() - scaled.height()) / 2;
        painter.drawPixmap(x, y, scaled);
        painter.fillRect(rect(), QColor(255, 255, 255, 24));
    }

    painter.setClipping(false);
    painter.setPen(QPen(QColor(145, 165, 180, 115), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 18, 18);
    QWidget::paintEvent(event);
}
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
    resize(900, 640);
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
    loadLocalSettings();
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

    const QString userName = protocolText(response->m_userName);
    const QString content = protocolText(response->szbuf);
    addMessageBubble(0, userName, content, QString());
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
        const QString userName = protocolText(response->m_users[i].m_userName);
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
        item->setIcon(QIcon(avatarFor(userId, userName)));
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
        ? protocolText(response->m_receiverName)
        : protocolText(response->m_senderName);
    if (peerId > 0 && !peerName.isEmpty() && !m_onlineUsers.contains(peerId)) {
        m_onlineUsers.insert(peerId, peerName);
    }

    const QString senderName = protocolText(response->m_senderName);
    const QString content = protocolText(response->szbuf);
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

    m_messages.clear();
    m_lastTimeSeparator.clear();
    while (QLayoutItem* item = m_messageLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_messageLayout->addStretch();

    for (int i = 0; i < response->m_messageCount && i < CHATHISTORYNUM; ++i) {
        const ChatHistoryInfo& item = response->m_messages[i];
        appendConversationLine(item.m_senderId,
                               protocolText(item.m_senderName),
                               protocolText(item.szbuf),
                               protocolText(item.m_createdAt));
    }
}

void Chat::handleProfileUpdate(STRU_PROFILE_UPDATE_RS* response)
{
    if (!response) {
        return;
    }

    QString message;
    if (response->m_szResult == _profile_update_success) {
        m_currentUserName = protocolText(response->m_szName);
        message = QStringLiteral("个人信息已更新");
        rebuildMessages();
        requestOnlineUsers();
    } else if (response->m_szResult == _profile_update_name_exists) {
        message = QStringLiteral("用户名已存在");
    } else if (response->m_szResult == _profile_update_invalid) {
        message = QStringLiteral("请填写新的用户名或密码");
    } else {
        message = QStringLiteral("个人信息更新失败");
    }
    m_peerLabel->setText(message);
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

void Chat::reloadLocalSettings()
{
    loadLocalSettings();
}

void Chat::sendMessage()
{
    const QString message = m_inputEdit->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    sendTextMessage(message);
}

void Chat::sendTextMessage(const QString& message)
{
    if (!m_kernel || !m_kernel->isConnected()) {
        return;
    }
    if (m_currentUserId <= 0 || m_currentPeerId <= 0) {
        m_peerLabel->setText(QStringLiteral("请先选择左侧在线用户"));
        return;
    }
    if (message.trimmed().isEmpty()) {
        return;
    }
    STRU_PRIVATE_CHAT_RQ request;
    request.m_senderId = m_currentUserId;
    request.m_receiverId = m_currentPeerId;
    copyProtocolText(request.m_senderName, MAXSIZE, m_currentUserName);
    copyProtocolText(request.m_receiverName, MAXSIZE, m_currentPeerName);
    copyProtocolText(request.szbuf, MAXSENDMESSSAGE, message);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));

    appendConversationLine(m_currentUserId, m_currentUserName, message, QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
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
    m_userList->setIconSize(QSize(34, 34));
    connect(m_userList, &QListWidget::itemClicked, this, &Chat::onUserActivated);
    sideLayout->addWidget(m_userList, 1);
    rootLayout->addLayout(sideLayout, 0);

    auto* conversationLayout = new QVBoxLayout;
    conversationLayout->setSpacing(12);

    auto* topBar = new QHBoxLayout;
    m_peerLabel = new QLabel(QStringLiteral("选择左侧在线用户开始私聊"), this);
    m_peerLabel->setObjectName(QStringLiteral("chatPeerLabel"));
    m_settingsButton = new QPushButton(this);
    m_settingsButton->setObjectName(QStringLiteral("settingsButton"));
    m_settingsButton->setIcon(makeGearIcon(QColor(18, 78, 98)));
    m_settingsButton->setIconSize(QSize(22, 22));
    m_settingsButton->setToolTip(QStringLiteral("聊天外观设置"));
    connect(m_settingsButton, &QPushButton::clicked, this, &Chat::openSettings);
    topBar->addWidget(m_peerLabel, 1);
    topBar->addWidget(m_settingsButton);
    conversationLayout->addLayout(topBar);

    m_conversationPanel = new ConversationPanel(this);
    auto* panelLayout = new QVBoxLayout(m_conversationPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(m_conversationPanel);
    m_scrollArea->setObjectName(QStringLiteral("messageScrollArea"));
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_messageContainer = new QWidget(m_scrollArea);
    m_messageLayout = new QVBoxLayout(m_messageContainer);
    m_messageLayout->setContentsMargins(18, 18, 18, 18);
    m_messageLayout->setSpacing(12);
    m_messageLayout->addStretch();
    m_scrollArea->setWidget(m_messageContainer);
    panelLayout->addWidget(m_scrollArea);
    conversationLayout->addWidget(m_conversationPanel, 1);

    auto* inputLayout = new QHBoxLayout;
    inputLayout->setSpacing(10);
    m_moreButton = new QPushButton(this);
    m_moreButton->setObjectName(QStringLiteral("moreButton"));
    m_moreButton->setIcon(makePlusIcon(QColor(22, 50, 79)));
    m_moreButton->setIconSize(QSize(24, 24));
    m_moreButton->setToolTip(QStringLiteral("更多发送方式"));
    connect(m_moreButton, &QPushButton::clicked, this, &Chat::toggleAttachmentPanel);
    m_emojiButton = new QPushButton(this);
    m_emojiButton->setObjectName(QStringLiteral("emojiButton"));
    m_emojiButton->setIcon(makeEmojiIcon(QColor(22, 50, 79)));
    m_emojiButton->setIconSize(QSize(24, 24));
    m_emojiButton->setToolTip(QStringLiteral("表情"));
    connect(m_emojiButton, &QPushButton::clicked, this, &Chat::toggleEmojiPanel);
    m_inputEdit = new QLineEdit(this);
    m_inputEdit->setPlaceholderText(QStringLiteral("输入私聊消息后按回车发送"));
    m_sendButton = new QPushButton(QStringLiteral("发送"), this);
    m_sendButton->setAutoDefault(true);
    connect(m_sendButton, &QPushButton::clicked, this, &Chat::sendMessage);
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &Chat::sendMessage);
    inputLayout->addWidget(m_moreButton);
    inputLayout->addWidget(m_emojiButton);
    inputLayout->addWidget(m_inputEdit, 1);
    inputLayout->addWidget(m_sendButton);
    conversationLayout->addLayout(inputLayout);

    m_attachmentPanel = new QWidget(this);
    m_attachmentPanel->setObjectName(QStringLiteral("attachmentPanel"));
    auto* attachmentGrid = new QGridLayout(m_attachmentPanel);
    attachmentGrid->setContentsMargins(14, 14, 14, 14);
    attachmentGrid->setSpacing(12);
    struct AttachmentAction {
        QString id;
        QString title;
        QString iconKind;
    };
    const QList<AttachmentAction> actions = {
        {QStringLiteral("localFile"), QStringLiteral("本地文件"), QStringLiteral("file")},
        {QStringLiteral("cloudFile"), QStringLiteral("网盘文件"), QStringLiteral("cloud")},
        {QStringLiteral("camera"), QStringLiteral("拍照"), QStringLiteral("camera")},
        {QStringLiteral("image"), QStringLiteral("图片"), QStringLiteral("image")},
        {QStringLiteral("video"), QStringLiteral("视频"), QStringLiteral("video")}
    };
    for (int i = 0; i < actions.size(); ++i) {
        const AttachmentAction action = actions.at(i);
        auto* button = new QPushButton(action.title, m_attachmentPanel);
        button->setObjectName(QStringLiteral("attachmentButton"));
        button->setIcon(makeAttachmentIcon(action.iconKind));
        button->setIconSize(QSize(42, 42));
        button->setToolTip(action.title);
        connect(button, &QPushButton::clicked, this, [this, action]() {
            handleAttachmentAction(action.id);
        });
        attachmentGrid->addWidget(button, i / 5, i % 5);
    }
    m_attachmentPanel->hide();
    conversationLayout->addWidget(m_attachmentPanel);

    m_emojiPanel = new QWidget(this);
    m_emojiPanel->setObjectName(QStringLiteral("emojiPanel"));
    auto* emojiGrid = new QGridLayout(m_emojiPanel);
    emojiGrid->setContentsMargins(14, 12, 14, 12);
    emojiGrid->setSpacing(10);
    const QStringList emojis = emojiChoices();
    for (int i = 0; i < emojis.size(); ++i) {
        const QString emoji = emojis.at(i);
        auto* button = new QPushButton(emoji, m_emojiPanel);
        button->setObjectName(QStringLiteral("emojiChoiceButton"));
        button->setToolTip(QStringLiteral("发送表情 %1").arg(emoji));
        connect(button, &QPushButton::clicked, this, [this, emoji]() {
            handleEmojiPicked(emoji);
        });
        emojiGrid->addWidget(button, i / 10, i % 10);
    }
    m_emojiPanel->hide();
    conversationLayout->addWidget(m_emojiPanel);
    rootLayout->addLayout(conversationLayout, 1);
}

void Chat::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QWidget#chatWindow { background: #eef4f6; color: #16324f; }"
        "QLabel { background: transparent; }"
        "QLabel#chatHeader { font-size: 22px; font-weight: 700; }"
        "QLabel#chatPeerLabel { font-size: 18px; font-weight: 700; }"
        "QListWidget#onlineUserList { min-width: 210px; max-width: 260px; border: 1px solid #d0dce6; border-radius: 8px; background: rgba(255,255,255,0.92); padding: 6px; }"
        "QListWidget#onlineUserList::item { min-height: 42px; padding: 7px 8px; border-radius: 6px; }"
        "QListWidget#onlineUserList::item:selected { background: #d8edf2; color: #16324f; }"
        "QWidget#conversationPanel { border: 1px solid rgba(145,165,180,0.45); border-radius: 18px; }"
        "QScrollArea#messageScrollArea, QScrollArea#messageScrollArea > QWidget > QWidget { background: transparent; border: none; }"
        "QLineEdit { min-height: 40px; border-radius: 14px; padding: 0 12px; border: 1px solid #d0dce6; background: rgba(255,255,255,0.94); }"
        "QPushButton { min-width: 88px; min-height: 40px; border-radius: 14px; background: #1f7a8c; color: white; font-weight: 600; border: none; }"
        "QPushButton:hover { background: #165f6d; }"
        "QPushButton#moreButton, QPushButton#emojiButton { min-width: 40px; max-width: 40px; min-height: 40px; max-height: 40px; border-radius: 20px; background: rgba(255,255,255,0.88); color: #16324f; border: 1px solid rgba(134,154,170,0.36); }"
        "QPushButton#moreButton:hover, QPushButton#emojiButton:hover { background: rgba(255,255,255,0.98); }"
        "QPushButton#settingsButton { min-width: 44px; max-width: 44px; min-height: 44px; max-height: 44px; border-radius: 22px; background: rgba(255,255,255,0.82); border: 1px solid rgba(134,154,170,0.32); }"
        "QPushButton#settingsButton:hover { background: rgba(255,255,255,0.96); }"
        "QWidget#attachmentPanel { background: rgba(255,255,255,0.72); border: 1px solid rgba(186,203,214,0.62); border-radius: 18px; }"
        "QPushButton#attachmentButton { min-width: 92px; min-height: 74px; border-radius: 16px; background: rgba(255,255,255,0.76); color: #16324f; border: 1px solid rgba(204,216,226,0.78); text-align: center; font-weight: 700; }"
        "QPushButton#attachmentButton:hover { background: #edf5f7; border-color: rgba(31,122,140,0.38); }"
        "QWidget#emojiPanel { background: rgba(255,255,255,0.72); border: 1px solid rgba(186,203,214,0.62); border-radius: 18px; }"
        "QPushButton#emojiChoiceButton { min-width: 44px; max-width: 44px; min-height: 44px; max-height: 44px; border-radius: 22px; background: rgba(255,255,255,0.82); color: #16324f; border: 1px solid rgba(204,216,226,0.72); font-size: 22px; padding: 0; }"
        "QPushButton#emojiChoiceButton:hover { background: #edf5f7; border-color: rgba(31,122,140,0.38); }"
        "QFrame.messageBubble { border-radius: 18px; border: 1px solid rgba(255,255,255,0.50); }"));
    applyConversationBackground();
}

void Chat::loadLocalSettings()
{
    QSettings settings;
    m_avatarPath = settings.value(settingsKey(m_currentUserId, QStringLiteral("avatarPath"))).toString();
    m_backgroundImagePath = settings.value(settingsKey(m_currentUserId, QStringLiteral("backgroundImagePath"))).toString();
    m_backgroundColor = settings.value(settingsKey(m_currentUserId, QStringLiteral("backgroundColor")), m_backgroundColor).toString();
    m_backgroundTheme = settings.value(settingsKey(m_currentUserId, QStringLiteral("backgroundTheme")), m_backgroundTheme).toString();
    m_patText = settings.value(settingsKey(m_currentUserId, QStringLiteral("patText")), m_patText).toString();
    if (m_avatarPreview) {
        m_avatarPreview->setPixmap(avatarFor(m_currentUserId, m_currentUserName));
    }
    if (m_backgroundLabel) {
        m_backgroundLabel->setText(m_backgroundImagePath.isEmpty()
            ? QStringLiteral("纯色背景：%1").arg(m_backgroundColor)
            : QFileInfo(m_backgroundImagePath).fileName());
    }
    applyConversationBackground();
    rebuildMessages();
}

void Chat::saveLocalSettings()
{
    QSettings settings;
    settings.setValue(settingsKey(m_currentUserId, QStringLiteral("avatarPath")), m_avatarPath);
    settings.setValue(settingsKey(m_currentUserId, QStringLiteral("backgroundImagePath")), m_backgroundImagePath);
    settings.setValue(settingsKey(m_currentUserId, QStringLiteral("backgroundColor")), m_backgroundColor);
    settings.setValue(settingsKey(m_currentUserId, QStringLiteral("backgroundTheme")), m_backgroundTheme);
    settings.setValue(settingsKey(m_currentUserId, QStringLiteral("patText")), m_patText);
}

void Chat::applyConversationBackground()
{
    if (!m_conversationPanel) {
        return;
    }
    m_conversationPanel->setBackground(m_backgroundColor, m_backgroundImagePath);
}

void Chat::applyBackgroundTheme(const QString& themeId)
{
    const ChatTheme theme = chatThemeById(themeId);
    m_backgroundTheme = theme.id;
    m_backgroundColor = theme.background;
    m_backgroundImagePath.clear();
    saveLocalSettings();
    if (m_backgroundLabel) {
        m_backgroundLabel->setText(QStringLiteral("当前组合：%1").arg(theme.name));
    }
    applyConversationBackground();
    rebuildMessages();
}

void Chat::rebuildMessages()
{
    if (!m_messageLayout) {
        return;
    }
    const QList<MessageEntry> messages = m_messages;
    m_messages.clear();
    m_lastTimeSeparator.clear();
    while (QLayoutItem* item = m_messageLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_messageLayout->addStretch();
    for (const MessageEntry& entry : messages) {
        appendConversationLine(entry.senderId, entry.senderName, entry.content, entry.createdAt);
    }
}

void Chat::openSettings()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("聊天设置"));
    dialog.setMinimumWidth(660);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 22, 22, 22);
    layout->setSpacing(14);

    auto* title = new QLabel(QStringLiteral("聊天设置"), &dialog);
    title->setObjectName(QStringLiteral("dialogTitle"));
    auto* subtitle = new QLabel(QStringLiteral("背景和拍一拍提示分开设置"), &dialog);
    subtitle->setObjectName(QStringLiteral("dialogSubtitle"));

    auto* backgroundCard = new QFrame(&dialog);
    backgroundCard->setObjectName(QStringLiteral("settingsCard"));
    auto* backgroundLayout = new QVBoxLayout(backgroundCard);
    backgroundLayout->setContentsMargins(16, 16, 16, 16);
    backgroundLayout->setSpacing(12);
    auto* backgroundTitle = new QLabel(QStringLiteral("背景组合"), backgroundCard);
    backgroundTitle->setObjectName(QStringLiteral("sectionTitle"));
    const ChatTheme activeTheme = chatThemeById(m_backgroundTheme);
    m_backgroundLabel = new QLabel(m_backgroundImagePath.isEmpty()
        ? QStringLiteral("当前组合：%1").arg(activeTheme.name)
        : QStringLiteral("自定义图片：%1").arg(QFileInfo(m_backgroundImagePath).fileName()), backgroundCard);
    m_backgroundLabel->setObjectName(QStringLiteral("sectionHint"));

    auto* themeGrid = new QGridLayout;
    themeGrid->setSpacing(10);
    const QList<ChatTheme> themes = chatThemes();
    for (int i = 0; i < themes.size(); ++i) {
        const ChatTheme theme = themes.at(i);
        auto* themeButton = new QPushButton(theme.name, backgroundCard);
        themeButton->setObjectName(QStringLiteral("themeButton"));
        themeButton->setStyleSheet(QStringLiteral(
            "QPushButton#themeButton { min-height: 54px; border-radius: 16px; border: 1px solid rgba(151,164,174,0.38); "
            "background: %1; color: #24384c; font-weight: 800; }"
            "QPushButton#themeButton:hover { border: 2px solid rgba(11,135,147,0.48); }").arg(theme.background));
        connect(themeButton, &QPushButton::clicked, this, [this, theme]() {
            applyBackgroundTheme(theme.id);
        });
        themeGrid->addWidget(themeButton, i / 2, i % 2);
    }

    auto* colorButton = new QPushButton(QStringLiteral("纯色背景"), &dialog);
    colorButton->setObjectName(QStringLiteral("primaryButton"));
    connect(colorButton, &QPushButton::clicked, this, &Chat::chooseBackgroundColor);
    auto* imageButton = new QPushButton(QStringLiteral("上传背景图片"), &dialog);
    imageButton->setObjectName(QStringLiteral("primaryButton"));
    connect(imageButton, &QPushButton::clicked, this, &Chat::chooseBackgroundImage);
    auto* backgroundButtonRow = new QHBoxLayout;
    backgroundButtonRow->addWidget(colorButton);
    backgroundButtonRow->addWidget(imageButton);

    backgroundLayout->addWidget(backgroundTitle);
    backgroundLayout->addWidget(m_backgroundLabel);
    backgroundLayout->addLayout(themeGrid);
    backgroundLayout->addLayout(backgroundButtonRow);

    auto* patCard = new QFrame(&dialog);
    patCard->setObjectName(QStringLiteral("settingsCard"));
    auto* patLayout = new QVBoxLayout(patCard);
    patLayout->setContentsMargins(16, 16, 16, 16);
    patLayout->setSpacing(12);
    auto* patTitle = new QLabel(QStringLiteral("拍一拍"), patCard);
    patTitle->setObjectName(QStringLiteral("sectionTitle"));
    auto* patHint = new QLabel(QStringLiteral("点击聊天头像时，聊天记录会显示拍一拍提示，并在背景上播放默认弹幕特效。"), patCard);
    patHint->setWordWrap(true);
    patHint->setObjectName(QStringLiteral("sectionHint"));
    auto* patEdit = new QLineEdit(m_patText, patCard);
    patEdit->setPlaceholderText(QStringLiteral("例如：轻轻拍了拍，留下了一阵温柔的回声"));

    auto* savePatButton = new QPushButton(QStringLiteral("保存拍一拍"), patCard);
    savePatButton->setObjectName(QStringLiteral("primaryButton"));
    connect(savePatButton, &QPushButton::clicked, this, [this, patEdit]() {
        const QString text = patEdit->text().trimmed();
        m_patText = text.isEmpty() ? QStringLiteral("轻轻拍了拍，留下了一阵温柔的回声") : text;
        saveLocalSettings();
    });
    patLayout->addWidget(patTitle);
    patLayout->addWidget(patHint);
    patLayout->addWidget(patEdit);
    patLayout->addWidget(savePatButton);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    if (auto* closeButton = buttons->button(QDialogButtonBox::Close)) {
        closeButton->setText(QStringLiteral("关闭"));
    }
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::accept);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addWidget(backgroundCard);
    layout->addWidget(patCard);
    layout->addWidget(buttons);

    dialog.setStyleSheet(QStringLiteral(
        "QDialog { background: #eef4f6; color: #16324f; }"
        "QLabel#dialogTitle { font-size: 24px; font-weight: 800; }"
        "QLabel#dialogSubtitle, QLabel#sectionHint { color: #5f7282; font-size: 13px; }"
        "QLabel#sectionTitle { font-size: 15px; font-weight: 800; }"
        "QFrame#settingsCard { background: rgba(255,255,255,0.76); border: 1px solid rgba(186,203,214,0.72); border-radius: 18px; }"
        "QLineEdit { min-height: 38px; border-radius: 12px; padding: 0 12px; border: 1px solid #c9d6e2; background: rgba(255,255,255,0.94); }"
        "QPushButton { min-height: 36px; border-radius: 12px; border: none; padding: 0 14px; font-weight: 700; }"
        "QPushButton#primaryButton { background: #0b8793; color: white; }"
        "QPushButton#primaryButton:hover { background: #086a74; }"
        "QPushButton#softButton { background: rgba(219,231,240,0.92); color: #16324f; }"));
    dialog.exec();

    m_avatarPreview = nullptr;
    m_avatarPathLabel = nullptr;
    m_backgroundLabel = nullptr;
}

void Chat::chooseBackgroundImage()
{
    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("选择聊天背景"), QString(),
                                                      QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (path.isEmpty()) {
        return;
    }
    ImageCropDialog cropDialog(path, 16.0 / 9.0, QSize(1920, 1080), this);
    if (cropDialog.exec() != QDialog::Accepted) {
        return;
    }
    const QString croppedPath = saveCroppedImage(cropDialog.croppedPixmap(), QStringLiteral("chat_background"));
    if (croppedPath.isEmpty()) {
        return;
    }
    m_backgroundImagePath = croppedPath;
    saveLocalSettings();
    if (m_backgroundLabel) {
        m_backgroundLabel->setText(QStringLiteral("自定义图片：%1").arg(QFileInfo(croppedPath).fileName()));
    }
    applyConversationBackground();
    rebuildMessages();
}

void Chat::chooseBackgroundColor()
{
    const QColor color = QColorDialog::getColor(QColor(m_backgroundColor), this, QStringLiteral("选择聊天背景色"));
    if (!color.isValid()) {
        return;
    }
    m_backgroundColor = color.name();
    m_backgroundImagePath.clear();
    m_backgroundTheme = QStringLiteral("custom");
    saveLocalSettings();
    if (m_backgroundLabel) {
        m_backgroundLabel->setText(QStringLiteral("纯色背景：%1").arg(m_backgroundColor));
    }
    applyConversationBackground();
    rebuildMessages();
}

void Chat::toggleAttachmentPanel()
{
    if (!m_attachmentPanel) {
        return;
    }
    m_attachmentPanel->setVisible(!m_attachmentPanel->isVisible());
    if (m_attachmentPanel->isVisible() && m_emojiPanel) {
        m_emojiPanel->hide();
    }
}

void Chat::toggleEmojiPanel()
{
    if (!m_emojiPanel) {
        return;
    }
    m_emojiPanel->setVisible(!m_emojiPanel->isVisible());
    if (m_emojiPanel->isVisible() && m_attachmentPanel) {
        m_attachmentPanel->hide();
    }
}

void Chat::handleEmojiPicked(const QString& emoji)
{
    if (emoji.trimmed().isEmpty()) {
        return;
    }
    sendTextMessage(emoji);
    if (m_emojiPanel) {
        m_emojiPanel->hide();
    }
}

void Chat::handleAttachmentAction(const QString& action)
{
    if (m_currentUserId <= 0 || m_currentPeerId <= 0) {
        m_peerLabel->setText(QStringLiteral("请先选择左侧在线用户"));
        return;
    }

    QString path;
    QString message;
    if (action == QStringLiteral("localFile")) {
        path = QFileDialog::getOpenFileName(this, QStringLiteral("选择要发送的本地文件"));
        if (path.isEmpty()) {
            return;
        }
        message = QStringLiteral("[文件] %1").arg(QFileInfo(path).fileName());
    } else if (action == QStringLiteral("image")) {
        path = QFileDialog::getOpenFileName(this, QStringLiteral("选择图片"), QString(),
                                            QStringLiteral("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
        if (path.isEmpty()) {
            return;
        }
        message = QStringLiteral("[图片] %1").arg(QFileInfo(path).fileName());
    } else if (action == QStringLiteral("video")) {
        path = QFileDialog::getOpenFileName(this, QStringLiteral("选择视频"), QString(),
                                            QStringLiteral("Videos (*.mp4 *.mov *.m4v *.avi *.mkv)"));
        if (path.isEmpty()) {
            return;
        }
        message = QStringLiteral("[视频] %1").arg(QFileInfo(path).fileName());
    } else if (action == QStringLiteral("cloudFile")) {
        QMessageBox::information(this, QStringLiteral("网盘文件"), QStringLiteral("网盘文件选择入口已预留，后续可接入文件列表。"));
        message = QStringLiteral("[网盘文件]");
    } else if (action == QStringLiteral("camera")) {
        QMessageBox::information(this, QStringLiteral("拍照"), QStringLiteral("拍照入口已预留，后续可接入摄像头采集。"));
        message = QStringLiteral("[拍照]");
    }

    if (!message.isEmpty()) {
        sendTextMessage(message);
        if (m_attachmentPanel) {
            m_attachmentPanel->hide();
        }
    }
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
    m_messages.clear();
    m_lastTimeSeparator.clear();
    while (QLayoutItem* item = m_messageLayout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_messageLayout->addStretch();
    setUnread(peerId, false);
    requestHistory(peerId);
    m_inputEdit->setFocus();
}

void Chat::appendConversationLine(long long senderId, const QString& senderName, const QString& content, const QString& createdAt)
{
    MessageEntry entry;
    entry.senderId = senderId;
    entry.senderName = senderName;
    entry.content = content;
    entry.createdAt = createdAt.trimmed().isEmpty()
        ? QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
        : createdAt;
    m_messages.append(entry);
    if (content.startsWith(patMessagePrefix())) {
        addSystemNotice(patDisplayText(content));
        return;
    }
    addMessageBubble(senderId, senderName, content, entry.createdAt);
    if (isEmojiMessage(content)) {
        showEmojiBurst(content.trimmed());
    }
}

bool Chat::isEmojiMessage(const QString& content) const
{
    return emojiChoices().contains(content.trimmed());
}

bool Chat::shouldShowTimeSeparator(const QString& createdAt) const
{
    if (createdAt.trimmed().isEmpty()) {
        return false;
    }
    if (m_lastTimeSeparator.isEmpty()) {
        return true;
    }

    const QDateTime previous = QDateTime::fromString(m_lastTimeSeparator, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    const QDateTime current = QDateTime::fromString(createdAt, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (!previous.isValid() || !current.isValid()) {
        return m_lastTimeSeparator.left(16) != createdAt.left(16);
    }
    return previous.secsTo(current) >= 5 * 60;
}

void Chat::addTimeSeparator(const QString& createdAt)
{
    if (!m_messageLayout || createdAt.trimmed().isEmpty()) {
        return;
    }
    auto* row = new QWidget(m_messageContainer);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 4, 0, 2);
    auto* label = new QLabel(createdAt.left(16), row);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QStringLiteral(
        "QLabel { color: rgba(30,48,62,0.60); background: rgba(255,255,255,0.46); "
        "border: 1px solid rgba(255,255,255,0.52); border-radius: 11px; padding: 3px 10px; font-size: 11px; }"));
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();
    const int stretchIndex = qMax(0, m_messageLayout->count() - 1);
    m_messageLayout->insertWidget(stretchIndex, row);
    m_lastTimeSeparator = createdAt;
}

void Chat::addSystemNotice(const QString& content)
{
    if (!m_messageLayout || content.trimmed().isEmpty()) {
        return;
    }
    auto* row = new QWidget(m_messageContainer);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 6, 0, 6);
    auto* label = new QLabel(content, row);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setMaximumWidth(520);
    label->setStyleSheet(QStringLiteral(
        "QLabel { color: rgba(42,55,68,0.72); background: rgba(255,255,255,0.46); "
        "border: 1px solid rgba(255,255,255,0.54); border-radius: 14px; padding: 5px 13px; "
        "font-size: 12px; font-weight: 700; }"));
    layout->addStretch();
    layout->addWidget(label);
    layout->addStretch();
    const int stretchIndex = qMax(0, m_messageLayout->count() - 1);
    m_messageLayout->insertWidget(stretchIndex, row);
}

void Chat::showDanmakuEditor(const QString& senderName)
{
    bool ok = false;
    const QString text = QInputDialog::getText(this,
                                              QStringLiteral("编辑弹幕"),
                                              QStringLiteral("发送到聊天背景上的弹幕"),
                                              QLineEdit::Normal,
                                              QStringLiteral("%1 的消息真不错").arg(senderName.section(QStringLiteral("（"), 0, 0)),
                                              &ok).trimmed();
    if (ok && !text.isEmpty()) {
        showDanmaku(text);
    }
}

void Chat::showDanmaku(const QString& text)
{
    if (!m_conversationPanel || text.trimmed().isEmpty()) {
        return;
    }

    auto* label = new QLabel(text, m_conversationPanel);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    const QString style = QStringLiteral(
        "QLabel { color: rgba(22,50,79,0.92); background: rgba(255,255,255,0.62); "
        "border: 1px solid rgba(255,255,255,0.68); border-radius: 15px; padding: 5px 14px; "
        "font-size: 13px; font-weight: 700; }");
    label->setStyleSheet(style);
    label->adjustSize();
    const int yLimit = qMax(1, m_conversationPanel->height() - label->height() - 28);
    const int y = QRandomGenerator::global()->bounded(18, qMax(19, yLimit));
    const QPoint start(m_conversationPanel->width() + 20, y);
    const QPoint end(-label->width() - 30, y);
    label->move(start);
    label->show();
    label->raise();

    auto* animation = new QPropertyAnimation(label, "pos", label);
    animation->setDuration(qMax(4200, m_conversationPanel->width() * 9));
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setEasingCurve(QEasingCurve::Linear);
    connect(animation, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void Chat::sendPatAction(const QString& targetName)
{
    if (m_currentUserId <= 0 || m_currentPeerId <= 0) {
        m_peerLabel->setText(QStringLiteral("请先选择左侧在线用户"));
        return;
    }
    const QString cleanTarget = targetName.section(QStringLiteral("（"), 0, 0).trimmed();
    const QString display = QStringLiteral("%1 拍了拍 %2：%3")
        .arg(m_currentUserName, cleanTarget.isEmpty() ? m_currentPeerName : cleanTarget, m_patText);
    sendTextMessage(patMessagePrefix() + display);
    showPatEffects(m_patText);
}

void Chat::showPatEffects(const QString& text)
{
    const QString effectText = text.trimmed().isEmpty() ? m_patText : text.trimmed();
    if (!m_conversationPanel) {
        return;
    }

    const QStringList petals = {
        QString::fromUtf8(u8"✿"),
        QString::fromUtf8(u8"❀"),
        QString::fromUtf8(u8"♡"),
        effectText
    };
    for (int i = 0; i < 16; ++i) {
        auto* label = new QLabel(petals.at(i % petals.size()), m_conversationPanel);
        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        label->setStyleSheet(QStringLiteral(
            "QLabel { color: rgba(125,72,98,0.84); background: rgba(255,246,250,0.56); "
            "border: 1px solid rgba(255,255,255,0.50); border-radius: 14px; padding: 4px 10px; "
            "font-size: 14px; font-weight: 800; }"));
        label->adjustSize();
        const int panelWidth = qMax(1, m_conversationPanel->width());
        const int x = QRandomGenerator::global()->bounded(panelWidth);
        const QPoint start(x, -label->height() - QRandomGenerator::global()->bounded(80));
        const QPoint end(qBound(0,
                                x + QRandomGenerator::global()->bounded(-90, 91),
                                qMax(0, panelWidth - label->width())),
                         m_conversationPanel->height() + 30);
        label->move(start);
        label->show();
        label->raise();

        auto* animation = new QPropertyAnimation(label, "pos", label);
        animation->setDuration(2600 + QRandomGenerator::global()->bounded(1500));
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->setEasingCurve(QEasingCurve::InOutSine);
        connect(animation, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void Chat::showEmojiBurst(const QString& emoji)
{
    if (!m_conversationPanel || emoji.trimmed().isEmpty()) {
        return;
    }
    const QPoint base(m_conversationPanel->width() - 120, m_conversationPanel->height() - 92);
    for (int i = 0; i < 6; ++i) {
        auto* label = new QLabel(emoji, m_conversationPanel);
        label->setAttribute(Qt::WA_TransparentForMouseEvents);
        label->setStyleSheet(QStringLiteral("QLabel { background: transparent; font-size: 28px; }"));
        label->adjustSize();
        const QPoint start(base.x() + QRandomGenerator::global()->bounded(-28, 29),
                           base.y() + QRandomGenerator::global()->bounded(-8, 18));
        const QPoint end(start.x() + QRandomGenerator::global()->bounded(-58, 59),
                         start.y() - QRandomGenerator::global()->bounded(80, 150));
        label->move(start);
        label->show();
        label->raise();
        auto* opacity = new QGraphicsOpacityEffect(label);
        label->setGraphicsEffect(opacity);
        auto* move = new QPropertyAnimation(label, "pos", label);
        move->setDuration(900 + i * 80);
        move->setStartValue(start);
        move->setEndValue(end);
        move->setEasingCurve(QEasingCurve::OutCubic);
        auto* fade = new QPropertyAnimation(opacity, "opacity", label);
        fade->setDuration(move->duration());
        fade->setStartValue(1.0);
        fade->setEndValue(0.0);
        connect(fade, &QPropertyAnimation::finished, label, &QLabel::deleteLater);
        move->start(QAbstractAnimation::DeleteWhenStopped);
        fade->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void Chat::addMessageBubble(long long senderId, const QString& senderName, const QString& content, const QString& createdAt)
{
    if (!m_messageLayout) {
        return;
    }
    const bool mine = senderId == m_currentUserId;
    const QString displayName = mine ? QStringLiteral("%1（我）").arg(m_currentUserName) : senderName;
    if (shouldShowTimeSeparator(createdAt)) {
        addTimeSeparator(createdAt);
    }

    auto* row = new QWidget(m_messageContainer);
    auto* rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(8);

    auto* avatar = new QPushButton(row);
    avatar->setObjectName(QStringLiteral("chatAvatarButton"));
    avatar->setFixedSize(44, 44);
    avatar->setIcon(QIcon(avatarFor(mine ? m_currentUserId : senderId, displayName)));
    avatar->setIconSize(QSize(44, 44));
    avatar->setToolTip(mine ? QString() : QStringLiteral("拍一拍"));
    avatar->setStyleSheet(QStringLiteral(
        "QPushButton#chatAvatarButton { min-width: 44px; max-width: 44px; min-height: 44px; max-height: 44px; "
        "border: none; border-radius: 14px; background: transparent; padding: 0; }"
        "QPushButton#chatAvatarButton:hover { background: rgba(255,255,255,0.28); }"));
    if (mine) {
        avatar->setCursor(Qt::ArrowCursor);
    } else {
        connect(avatar, &QPushButton::clicked, this, [this, displayName]() {
            sendPatAction(displayName);
        });
    }

    auto* messageColumn = new QWidget(row);
    auto* columnLayout = new QVBoxLayout(messageColumn);
    columnLayout->setContentsMargins(0, 0, 0, 0);
    columnLayout->setSpacing(4);
    auto* nameLabel = new QLabel(displayName, messageColumn);
    nameLabel->setAlignment(mine ? Qt::AlignRight : Qt::AlignLeft);

    auto* bubble = new QFrame(row);
    bubble->setObjectName(QStringLiteral("messageBubble"));
    bubble->setProperty("class", QStringLiteral("messageBubble"));
    bubble->setMaximumWidth(520);
    auto* shadow = new QGraphicsDropShadowEffect(bubble);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(36, 58, 76, 42));
    bubble->setGraphicsEffect(shadow);
    const ChatTheme theme = bubbleThemeForId(m_backgroundTheme);
    nameLabel->setStyleSheet(mine
        ? QStringLiteral("color: %1; font-size: 11px; font-weight: 700; padding: 0 4px;").arg(theme.mineMeta)
        : QStringLiteral("color: %1; font-size: 11px; font-weight: 700; padding: 0 4px;").arg(theme.peerMeta));
    bubble->setStyleSheet(mine
        ? QStringLiteral("QFrame#messageBubble { border-radius: 22px; border: 1px solid rgba(255,255,255,0.46); background: %1; }").arg(theme.mineBubble)
        : QStringLiteral("QFrame#messageBubble { border-radius: 22px; border: 1px solid rgba(255,255,255,0.68); background: %1; }").arg(theme.peerBubble));

    auto* bubbleLayout = new QVBoxLayout(bubble);
    bubbleLayout->setContentsMargins(15, 8, 15, 8);
    bubbleLayout->setSpacing(0);
    auto* text = new QLabel(content, bubble);
    text->setWordWrap(true);
    text->setTextInteractionFlags(Qt::TextSelectableByMouse);
    text->setStyleSheet(mine
        ? QStringLiteral("color: %1; line-height: 1.45;").arg(theme.mineText)
        : QStringLiteral("color: %1; line-height: 1.45;").arg(theme.peerText));
    bubbleLayout->addWidget(text);
    columnLayout->addWidget(nameLabel);
    columnLayout->addWidget(bubble, 0, mine ? Qt::AlignRight : Qt::AlignLeft);

    if (mine) {
        rowLayout->addStretch();
        rowLayout->addWidget(messageColumn);
        rowLayout->addWidget(avatar);
    } else {
        rowLayout->addWidget(avatar);
        rowLayout->addWidget(messageColumn);
        rowLayout->addStretch();
    }

    const int stretchIndex = qMax(0, m_messageLayout->count() - 1);
    m_messageLayout->insertWidget(stretchIndex, row);
    QTimer::singleShot(0, this, [this]() {
        if (m_scrollArea && m_scrollArea->verticalScrollBar()) {
            m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
        }
    });
}

QPixmap Chat::avatarFor(long long userId, const QString& userName) const
{
    QPixmap base;
    if (userId == m_currentUserId && !m_avatarPath.isEmpty() && QFileInfo::exists(m_avatarPath)) {
        base.load(m_avatarPath);
    }
    if (base.isNull()) {
        base = QPixmap(64, 64);
        base.fill(Qt::transparent);
        QPainter painter(&base);
        painter.setRenderHint(QPainter::Antialiasing);
        QLinearGradient gradient(0, 0, 64, 64);
        gradient.setColorAt(0, QColor(42, 111, 151));
        gradient.setColorAt(1, QColor(31, 122, 140));
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(base.rect().adjusted(4, 4, -4, -4));
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(22);
        painter.setFont(font);
        painter.drawText(base.rect(), Qt::AlignCenter, initials(userName));
    }
    return roundedAvatar(base, 64);
}

QPixmap Chat::roundedAvatar(const QPixmap& source, int size) const
{
    QPixmap scaled = source.scaled(size - 8, size - 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap result(size, size);
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(QRectF(4, 4, size - 8, size - 8), 15, 15);
    painter.setClipPath(path);
    painter.fillRect(QRect(4, 4, size - 8, size - 8), QColor(244, 248, 251));
    const int x = 4 + ((size - 8) - scaled.width()) / 2;
    const int y = 4 + ((size - 8) - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);
    painter.setClipping(false);
    QPen pen(QColor(233, 240, 247), 2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(QRectF(4, 4, size - 8, size - 8), 15, 15);
    return result;
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
        item->setIcon(QIcon(avatarFor(peerId, userName)));
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
