#include "tcpnet.h"

TCPNet::TCPNet(IKernel*pKernel)
{
    m_sockclient =0;
    m_hThread  = 0;
    m_bflagQuit = true;
    m_pKernel = pKernel;
    char* szip = nullptr;
}
TCPNet::~TCPNet()
{

}

bool TCPNet::connectServer(const char* szip ,short sport)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {

        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return false;
    }
    else
        printf("The Winsock 2.2 dll was found okay\n");

    m_sockclient = socket(AF_INET,SOCK_STREAM,0);
    if(INVALID_SOCKET == m_sockclient){
        disconnectServer("socket err\n");

    }

    sockaddr_in addrserver;
    addrserver.sin_family =AF_INET;
    addrserver.sin_addr.S_un.S_addr = inet_addr(szip);
    addrserver.sin_port = htons(sport);
    if(SOCKET_ERROR == connect(m_sockclient,(const sockaddr*)&addrserver,sizeof(addrserver))){
        disconnectServer("connect err\n");
        return false;
    }

    //创建线程--接收数据
    m_hThread = CreateThread(0,0,&ThreadProc,this,0,0);
    return true;
}

DWORD WINAPI TCPNet::ThreadProc(LPVOID lpvoid)
{
    TCPNet *pthis = (TCPNet*)lpvoid;
    pthis->recvData();
    return 0;
}
void TCPNet::disconnectServer(const char* szerr)
{
    m_bflagQuit =false;
    if(m_hThread){
        if(WAIT_TIMEOUT == WaitForSingleObject(m_hThread,100))
            TerminateThread(m_hThread,-1);

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    printf(szerr);
    if(m_sockclient){
        closesocket(m_sockclient);
        m_sockclient =0;
    }
    WSACleanup();

}
bool TCPNet::sendData(const char* szbuf,int nlen)
{
    if(!szbuf ||nlen <=0 )
        return false;
    //包大小
    if(send(m_sockclient,(char*)&nlen,sizeof(int),0)<=0)
        return false;

    //包内容
    if(send(m_sockclient,szbuf,nlen,0)<=0)
        return false;

    //序列化和反序列化--protobuf
    return true;
}
void TCPNet::recvData()
{
    int nPackSize;
    int nReadNum;
    char *pszbuf = NULL;
    while(m_bflagQuit){
       // 接收数据
       // 接收包大小
        nReadNum = recv(m_sockclient,(char*)&nPackSize,sizeof(int),0);
        if(nReadNum <=0){

            //服务器异常
            break;
        }
        if(nPackSize <=0)
            continue;

        pszbuf = new char[nPackSize];
        //接收包内容
        int offset = 0;
        while(nPackSize){
            nReadNum = recv(m_sockclient,pszbuf+offset,nPackSize,0);
            offset += nReadNum;
            nPackSize -= nReadNum;

        }

        // printf("server says:%s\n",pszbuf);
        m_pKernel->dealData(pszbuf);

        delete []pszbuf;
        pszbuf = NULL;



    }
}
