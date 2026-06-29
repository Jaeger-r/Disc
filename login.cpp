#include "login.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QStyle>
#include <QVBoxLayout>

#include "kernel/tcpkernel.h"

Login::Login(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("loginWindow"));
    buildUi();
    applyStyle();
    setWindowTitle(QStringLiteral("Disk Login"));
    resize(760, 460);
}

Login::~Login() = default;

void Login::setKernel(IKernel* pKernel)
{
    m_pKernel = pKernel;
    setWindowIcon(QIcon(QStringLiteral(":/app/DiskClientIcon.png")));
}

QString Login::currentUserName() const
{
    return m_currentUserName;
}

void Login::slot_register(STRU_REGISTER_RS* psrr)
{
    const QString message = psrr && psrr->m_szResult == _register_success
        ? QStringLiteral("注册成功，请直接登录。")
        : QStringLiteral("注册失败，用户名或手机号可能已存在。");
    QMessageBox::information(this, QStringLiteral("注册"), message);
}

void Login::slot_connectionStateChanged(bool connected, const QString& reason)
{
    if (!m_connectionLabel) {
        return;
    }

    if (connected) {
        m_connectionLabel->setText(QStringLiteral("已连接服务端"));
        m_connectionLabel->setProperty("state", QStringLiteral("connected"));
    } else {
        m_connectionLabel->setText(reason.trimmed().isEmpty()
            ? QStringLiteral("未连接服务端")
            : QStringLiteral("未连接服务端：%1").arg(reason));
        m_connectionLabel->setProperty("state", QStringLiteral("disconnected"));
    }
    m_connectionLabel->style()->unpolish(m_connectionLabel);
    m_connectionLabel->style()->polish(m_connectionLabel);
}

void Login::onRegisterClicked()
{
    qInfo() << "Register button clicked";
    if (!m_pKernel || (!m_pKernel->isConnected() && !m_pKernel->connect())) {
        QMessageBox::warning(this, QStringLiteral("连接失败"), QStringLiteral("客户端尚未连接到服务端。"));
        return;
    }

    STRU_REGISTER_RQ request;
    memset(request.m_szName, 0, sizeof(request.m_szName));
    memset(request.m_szPassWord, 0, sizeof(request.m_szPassWord));
    const QString tel = m_registerTelEdit->text().trimmed();
    const QString user = m_registerUserEdit->text().trimmed();
    const QString password = m_registerPasswordEdit->text();
    if (tel.isEmpty() || user.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("注册"), QStringLiteral("请填写手机号、用户名和密码。"));
        return;
    }
    qstrncpy(request.m_szName, user.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_szPassWord, password.toLocal8Bit().constData(), MAXSIZE);
    request.m_tel = tel.toLongLong();

    if (!m_pKernel->sendData(reinterpret_cast<char*>(&request), sizeof(request))) {
        QMessageBox::warning(this, QStringLiteral("注册"), QStringLiteral("注册请求发送失败。"));
    }
}

void Login::onLoginClicked()
{
    qInfo() << "Login button clicked";
    if (!m_pKernel || (!m_pKernel->isConnected() && !m_pKernel->connect())) {
        QMessageBox::warning(this, QStringLiteral("连接失败"), QStringLiteral("客户端尚未连接到服务端。"));
        return;
    }

    STRU_LOGIN_RQ request;
    memset(request.m_szName, 0, sizeof(request.m_szName));
    memset(request.m_szPassWord, 0, sizeof(request.m_szPassWord));
    m_currentUserName = m_loginUserEdit->text().trimmed();
    if (m_currentUserName.isEmpty() || m_loginPasswordEdit->text().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("登录"), QStringLiteral("请填写用户名和密码。"));
        return;
    }
    qstrncpy(request.m_szName, m_currentUserName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_szPassWord, m_loginPasswordEdit->text().toLocal8Bit().constData(), MAXSIZE);
    if (m_pKernel->sendData(reinterpret_cast<char*>(&request), sizeof(request))) {
        emit loginRequested();
    } else {
        QMessageBox::warning(this, QStringLiteral("登录"), QStringLiteral("登录请求发送失败。"));
    }
}

void Login::buildUi()
{
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(28, 28, 28, 28);
    rootLayout->setSpacing(20);

    auto* introCard = new QFrame(this);
    introCard->setObjectName(QStringLiteral("introCard"));
    auto* introLayout = new QVBoxLayout(introCard);
    introLayout->setContentsMargins(28, 28, 28, 28);
    introLayout->setSpacing(16);

    auto* title = new QLabel(QStringLiteral("Jaeger Disk"), introCard);
    title->setObjectName(QStringLiteral("titleLabel"));
    auto* subtitle = new QLabel(QStringLiteral("基于 Qt 的轻量级网盘客户端，支持登录、聊天、上传与下载。"), introCard);
    subtitle->setWordWrap(true);
    subtitle->setObjectName(QStringLiteral("subtitleLabel"));
    introLayout->addWidget(title);
    introLayout->addWidget(subtitle);
    introLayout->addStretch();

    auto* formCard = new QFrame(this);
    formCard->setObjectName(QStringLiteral("formCard"));
    auto* formCardLayout = new QVBoxLayout(formCard);
    formCardLayout->setContentsMargins(24, 24, 24, 24);
    formCardLayout->setSpacing(14);

    auto* header = new QLabel(QStringLiteral("账号入口"), formCard);
    header->setObjectName(QStringLiteral("headerLabel"));
    formCardLayout->addWidget(header);

    m_connectionLabel = new QLabel(QStringLiteral("正在连接服务端..."), formCard);
    m_connectionLabel->setObjectName(QStringLiteral("connectionLabel"));
    m_connectionLabel->setProperty("state", QStringLiteral("pending"));
    formCardLayout->addWidget(m_connectionLabel);

    m_tabWidget = new QTabWidget(formCard);
    formCardLayout->addWidget(m_tabWidget);

    auto* registerPage = new QWidget(m_tabWidget);
    registerPage->setObjectName(QStringLiteral("registerPage"));
    auto* registerLayout = new QVBoxLayout(registerPage);
    auto* registerForm = new QFormLayout;
    registerForm->setLabelAlignment(Qt::AlignLeft);
    registerForm->setSpacing(12);

    m_registerTelEdit = new QLineEdit(registerPage);
    m_registerTelEdit->setPlaceholderText(QStringLiteral("输入手机号"));
    m_registerUserEdit = new QLineEdit(registerPage);
    m_registerUserEdit->setPlaceholderText(QStringLiteral("输入用户名"));
    m_registerPasswordEdit = new QLineEdit(registerPage);
    m_registerPasswordEdit->setPlaceholderText(QStringLiteral("输入密码"));
    m_registerPasswordEdit->setEchoMode(QLineEdit::Password);
    registerForm->addRow(QStringLiteral("手机号"), m_registerTelEdit);
    registerForm->addRow(QStringLiteral("用户名"), m_registerUserEdit);
    registerForm->addRow(QStringLiteral("密码"), m_registerPasswordEdit);

    m_registerButton = new QPushButton(QStringLiteral("创建账号"), registerPage);
    m_registerButton->setAutoDefault(true);
    connect(m_registerButton, &QPushButton::clicked, this, &Login::onRegisterClicked);
    connect(m_registerTelEdit, &QLineEdit::returnPressed, this, &Login::onRegisterClicked);
    connect(m_registerUserEdit, &QLineEdit::returnPressed, this, &Login::onRegisterClicked);
    connect(m_registerPasswordEdit, &QLineEdit::returnPressed, this, &Login::onRegisterClicked);
    registerLayout->addLayout(registerForm);
    registerLayout->addStretch();
    registerLayout->addWidget(m_registerButton);

    auto* loginPage = new QWidget(m_tabWidget);
    loginPage->setObjectName(QStringLiteral("loginPage"));
    auto* loginLayout = new QVBoxLayout(loginPage);
    auto* loginForm = new QFormLayout;
    loginForm->setLabelAlignment(Qt::AlignLeft);
    loginForm->setSpacing(12);

    m_loginUserEdit = new QLineEdit(loginPage);
    m_loginUserEdit->setPlaceholderText(QStringLiteral("输入用户名"));
    m_loginPasswordEdit = new QLineEdit(loginPage);
    m_loginPasswordEdit->setPlaceholderText(QStringLiteral("输入密码"));
    m_loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginForm->addRow(QStringLiteral("用户名"), m_loginUserEdit);
    loginForm->addRow(QStringLiteral("密码"), m_loginPasswordEdit);

    m_loginButton = new QPushButton(QStringLiteral("登录网盘"), loginPage);
    m_loginButton->setAutoDefault(true);
    connect(m_loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(m_loginUserEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
    connect(m_loginPasswordEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
    loginLayout->addLayout(loginForm);
    loginLayout->addStretch();
    loginLayout->addWidget(m_loginButton);

    m_tabWidget->addTab(registerPage, QStringLiteral("注册"));
    m_tabWidget->addTab(loginPage, QStringLiteral("登录"));

    rootLayout->addWidget(introCard, 5);
    rootLayout->addWidget(formCard, 6);
}

void Login::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QWidget#loginWindow { background: #eef3f7; color: #16324f; }"
        "QLabel { background: transparent; }"
        "QFrame#introCard { border-radius: 24px; background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #16324f, stop:1 #2a6f97); }"
        "QFrame#formCard { border-radius: 24px; background: rgba(255,255,255,0.96); }"
        "QWidget#registerPage, QWidget#loginPage { background: transparent; }"
        "QLabel#titleLabel { color: white; font-size: 32px; font-weight: 700; }"
        "QLabel#subtitleLabel { color: rgba(255,255,255,0.82); font-size: 15px; line-height: 1.5; }"
        "QLabel#headerLabel { font-size: 22px; font-weight: 700; }"
        "QLabel#connectionLabel { min-height: 24px; font-size: 13px; font-weight: 600; color: #6b7280; }"
        "QLabel#connectionLabel[state=\"connected\"] { color: #0f8a5f; }"
        "QLabel#connectionLabel[state=\"disconnected\"] { color: #b42318; }"
        "QLineEdit { min-height: 38px; border-radius: 12px; padding: 0 12px; border: 1px solid #c9d6e2; background: #f8fbfd; }"
        "QPushButton { min-height: 42px; border-radius: 14px; border: none; background: #1f7a8c; color: white; font-weight: 600; }"
        "QPushButton:hover { background: #165f6d; }"
        "QTabWidget::pane { border: none; }"
        "QTabBar::tab { min-width: 120px; min-height: 34px; border-radius: 10px; padding: 0 12px; margin-right: 6px; background: #dbe7f0; }"
        "QTabBar::tab:selected { background: #16324f; color: white; }"));
}
