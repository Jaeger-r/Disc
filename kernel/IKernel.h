#ifndef IKERNEL_H
#define IKERNEL_H

#include <QtGlobal>

class IKernel{

public:
    IKernel(){

    }
    virtual ~IKernel(){

    }
public:
    virtual bool connect(const char* szip = "",quint16 sport = 0) = 0;
    virtual void disconnect(const char * szerr) = 0;
    virtual bool sendData(const char * szbuf,int nlen) = 0;
    virtual void dealData(const char * szbuf) = 0;
    virtual bool isConnected() const = 0;
};

#endif // IKERNEL_H
