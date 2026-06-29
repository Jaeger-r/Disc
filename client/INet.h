#ifndef INET_H
#define INET_H
#include <QtGlobal>
class INet
{
public:
    INet(){

    }
    virtual ~INet(){

    }
public:
    virtual bool connectServer(const char* szip = "",quint16 sport=0) = 0;
    virtual void disconnectServer(const char* szerr) = 0;
    virtual bool sendData(const char* szbuf,int nlen)= 0 ;

};
#endif // INET_H
