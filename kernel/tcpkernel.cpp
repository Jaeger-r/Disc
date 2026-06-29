#include "tcpkernel.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

TCPKernel::TCPKernel(QObject *parent)
    : QObject{parent}
{
    m_pTCPNet = new TCPNet(this, this);
    auto* tcpNet = dynamic_cast<TCPNet*>(m_pTCPNet);
    if (tcpNet) {
        QObject::connect(tcpNet, &TCPNet::connectionStateChanged, this, &TCPKernel::signal_connectionStateChanged);
    }

    const QString appDir = QCoreApplication::applicationDirPath();
    const QString configPath = QDir(appDir).filePath(QStringLiteral("diskclient.ini"));
    QSettings settings(configPath, QSettings::IniFormat);
    if (!QFileInfo::exists(configPath)) {
        settings.setValue(QStringLiteral("network/serverHost"), m_serverHost);
        settings.setValue(QStringLiteral("network/serverPort"), m_serverPort);
        settings.setValue(QStringLiteral("network/tlsEnabled"), m_tlsEnabled);
        settings.setValue(QStringLiteral("network/tlsCaPath"),
                          QDir(appDir).filePath(QStringLiteral("tls/ca.crt")));
        settings.sync();
    }
    const QString configuredHost =
        settings.value(QStringLiteral("network/serverHost"), m_serverHost).toString().trimmed();
    const quint16 configuredPort =
        static_cast<quint16>(settings.value(QStringLiteral("network/serverPort"), m_serverPort).toUInt());
    m_tlsEnabled = settings.value(QStringLiteral("network/tlsEnabled"), m_tlsEnabled).toBool();
    const QString configuredTlsCaPath =
        settings.value(QStringLiteral("network/tlsCaPath"),
                       QDir(appDir).filePath(QStringLiteral("tls/ca.crt"))).toString().trimmed();
    m_tlsCaPath = QDir::isAbsolutePath(configuredTlsCaPath)
        ? configuredTlsCaPath
        : QDir(appDir).filePath(configuredTlsCaPath);
    if (!configuredHost.isEmpty()) {
        m_serverHost = configuredHost;
    }
    if (configuredPort > 0) {
        m_serverPort = configuredPort;
    }

    if (auto* tcpNet = dynamic_cast<TCPNet*>(m_pTCPNet)) {
        tcpNet->configureTls(m_tlsEnabled, m_tlsCaPath);
    }
}

TCPKernel::~TCPKernel()
{
    delete m_pTCPNet;
}

bool TCPKernel::connect(const char *szip, quint16 sport)
{
    const QString requestedHost = QString::fromLocal8Bit(szip ? szip : "").trimmed();
    const QByteArray hostBytes =
        (requestedHost.isEmpty() ? m_serverHost : requestedHost).toLocal8Bit();
    const quint16 targetPort = sport > 0 ? sport : m_serverPort;
    if(m_pTCPNet->connectServer(hostBytes.constData(), targetPort)){
        return true;
    }else{
        return false;
    }
}

QString TCPKernel::appResourcePath(const QString& relativePath) const
{
    return QDir(QCoreApplication::applicationDirPath()).filePath(relativePath);
}

void TCPKernel::disconnect(const char *szerr)
{
    m_pTCPNet->disconnectServer(szerr);
}

bool TCPKernel::sendData(const char *szbuf, int nlen)
{
    if(!m_pTCPNet->sendData(szbuf,nlen)){
        return false;
    }
    return true;
}

void TCPKernel::dealData(const char *szbuf)
{
    switch(*szbuf){
        case _default_protocol_register_send:
            emit signal_register((STRU_REGISTER_RS*)szbuf);
            break;
        case _default_protocol_login_send:
            emit signal_login((STRU_LOGIN_RS*)szbuf);
            break;
        case _default_protocol_getfilelist_send:
            emit signal_getfilelist((STRU_GETFILELIST_RS*)szbuf);
            break;
        case _default_protocol_uploadfileinfo_send:
            emit signal_uploadfileinfo((STRU_UPLOADFILEINFO_RS*)szbuf);
            break;
        case _default_protocol_uoloadfileblock_send:
            emit signal_uploadfileblock((STRU_UPLOADFILEBLOCK_RS*)szbuf);
            break;
        case _default_protocol_deletefile_send:
            emit signal_deletefileinfo((STRU_DELETEFILE_RS*)szbuf);
            break;
        case _default_protocol_renamefile_send:
            emit signal_renamefileinfo((STRU_RENAMEFILE_RS*)szbuf);
            break;
        case _default_protocol_filesync_send:
            emit signal_filesync((STRU_FILESYNC_RS*)szbuf);
            break;
        case _default_protocol_downloadfileinfo_send:
            emit signal_downloadfileinfo((STRU_DOWNLOADFILEINFO_RS*)szbuf);
            break;
        case _default_protocol_downloadfileblock_send:
            emit signal_downloadfileblock((STRU_DOWNLOADFILEBLOCK_RS*)szbuf);
            break;
        case _default_protocol_chat_send:
            emit signal_chat((STRU_CHAT_RS*)szbuf);
            break;
        case _default_protocol_online_users_send:
            emit signal_onlineusers((STRU_ONLINE_USERS_RS*)szbuf);
            break;
        case _default_protocol_private_chat_send:
            emit signal_privatechat((STRU_PRIVATE_CHAT_RS*)szbuf);
            break;
        case _default_protocol_private_history_send:
            emit signal_privatehistory((STRU_PRIVATE_HISTORY_RS*)szbuf);
            break;
        case _default_protocol_transfercontrol_send:
            emit signal_transfercontrol((STRU_TRANSFERCONTROL_RS*)szbuf);
            break;
    }
}

bool TCPKernel::isConnected() const
{
    const auto* net = dynamic_cast<const TCPNet*>(m_pTCPNet);
    return net && net->isConnected();
}
