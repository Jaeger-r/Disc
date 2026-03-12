#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include <QObject>
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
    bool connect(const char* szip = IP,short sport = PORT);
    void disconnect(const char * szerr);
    bool sendData(const char * szbuf,int nlen);
    void dealData(const char * szbuf);
signals:
    void signal_register(STRU_REGISTER_RS*);
    void signal_login(STRU_LOGIN_RS*);
    void signal_getfilelist(STRU_GETFILELIST_RS*);
    void signal_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
    void signal_uploadfileblock(STRU_UPLOADFILEBLOCK_RS*);
    void signal_deletefileinfo(STRU_DELETEFILE_RS*);
private:
    INet*m_pTCPNet;
};

#endif // TCPKERNEL_H
