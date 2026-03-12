#ifndef TCPNET_H
#define TCPNET_H
#include <winsock2.h>
#include "INet.h"
#include <stdio.h>
#include <stdlib.h>
#include "tou.h"
#include "kernel/IKernel.h"
class TCPNet : public INet
{
public:
    TCPNet(IKernel*pKernel);
    ~TCPNet();
public:
    bool connectServer(const char* szip = IP,short sport = PORT);
    void disconnectServer(const char* szerr);
    bool sendData(const char* szbuf,int nlen);
    void recvData();
    static DWORD WINAPI ThreadProc(LPVOID lpvoid);
private:
    char* szip;
    SOCKET m_sockclient;
    HANDLE m_hThread;
    bool   m_bflagQuit;
    IKernel *m_pKernel;
};

#endif // TCPNET_H
