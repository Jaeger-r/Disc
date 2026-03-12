#ifndef TCPNET_H
#define TCPNET_H
#include <list>
#include "INet.h"
#define MAXNUM   1024

class TCPNet:public INet
{
public:
    TCPNet();
    ~TCPNet();
public:
    bool initNetWork(const char* szip = "127.0.0.1",short sport=1234);
    void unInitNetWork(const char* szerr);
    bool sendData(SOCKET sock,const char* szbuf,int nlen);
    void recvData();
    std::map<DWORD, SOCKET>& getThreadIdToSocketMap(); // 新增函数声明
public:
    static DWORD WINAPI ThreadProc(LPVOID lpvoid);
    static DWORD WINAPI ThreadRecv(LPVOID lpvoid);
public:
    SOCKET m_socklisten;
    std::list<HANDLE> m_lstThread;
    std::map<DWORD,SOCKET> m_mapThreadIdToSocket;
    bool  m_bflagQuit;
};

#endif // TCPNET_H
