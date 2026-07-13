#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include <QObject>
#include <QSettings>
#include "IKernel.h"
#include "../client/tcpnet.h"
#include "Packdef.h"
class TCPKernel : public QObject,public IKernel
{
    Q_OBJECT
public:
    explicit TCPKernel(QObject *parent = nullptr);
    ~TCPKernel();
public:
    bool connect(const char* szip = "",quint16 sport = 0) override;
    void disconnect(const char * szerr) override;
    bool sendData(const char * szbuf,int nlen) override;
    void dealData(const char * szbuf) override;
signals:
    void signal_register(STRU_REGISTER_RS*);
    void signal_login(STRU_LOGIN_RS*);
    void signal_getfilelist(STRU_GETFILELIST_RS*);
    void signal_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
    void signal_uploadfileblock(STRU_UPLOADFILEBLOCK_RS*);
    void signal_deletefileinfo(STRU_DELETEFILE_RS*);
    void signal_renamefileinfo(STRU_RENAMEFILE_RS*);
    void signal_filesync(STRU_FILESYNC_RS*);
    void signal_downloadfileinfo(STRU_DOWNLOADFILEINFO_RS*);
    void signal_downloadfileblock(STRU_DOWNLOADFILEBLOCK_RS*);
    void signal_chat(STRU_CHAT_RS*);
    void signal_onlineusers(STRU_ONLINE_USERS_RS*);
    void signal_privatechat(STRU_PRIVATE_CHAT_RS*);
    void signal_privatehistory(STRU_PRIVATE_HISTORY_RS*);
    void signal_profileupdate(STRU_PROFILE_UPDATE_RS*);
    void signal_transfercontrol(STRU_TRANSFERCONTROL_RS*);
    void signal_connectionStateChanged(bool connected, const QString& reason);
private:
    INet*m_pTCPNet;
    QString m_serverHost = QStringLiteral("127.0.0.1");
    quint16 m_serverPort = 1234;
    bool m_tlsEnabled = true;
    QString m_tlsCaPath;
public:
    bool isConnected() const override;
    QString appResourcePath(const QString& relativePath) const;
};

#endif // TCPKERNEL_H
