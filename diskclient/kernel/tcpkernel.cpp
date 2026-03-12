#include "tcpkernel.h"

TCPKernel::TCPKernel(QObject *parent)
    : QObject{parent}
{
    m_pTCPNet = new TCPNet(this);
}

TCPKernel::~TCPKernel()
{
    delete m_pTCPNet;
}

bool TCPKernel::connect(const char *szip, short sport)
{
    if(m_pTCPNet->connectServer(szip,sport)){
        return true;
    }else{
        return false;
    }
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
        case _default_protocol_deletefile_send:
            //emit signal_deletefileinfo((STRU_DELETEFILE_RS*)szbuf);
            break;
        case _default_protocol_chat_send:
            //emit signal_deletefileinfo((STRU_DELETEFILE_RS*)szbuf);
            break;
        case _default_protocol_uoloadfileblock_send:
            emit signal_uploadfileblock((STRU_UPLOADFILEBLOCK_RS*)szbuf);
            break;
    }
}
