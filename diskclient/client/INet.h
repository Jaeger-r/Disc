#ifndef INET_H
#define INET_H
#include "tou.h"
class INet
{
public:
    INet(){

    }
    virtual ~INet(){

    }
public:
    virtual bool connectServer(const char* szip = IP,short sport = PORT) = 0;
    virtual void disconnectServer(const char* szerr) = 0;
    virtual bool sendData(const char* szbuf,int nlen)= 0 ;
    virtual void recvData()= 0 ;

};
#endif // INET_H
