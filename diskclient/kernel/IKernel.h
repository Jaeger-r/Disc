#ifndef IKERNEL_H
#define IKERNEL_H
#include "tou.h"

class IKernel{

public:
    IKernel(){

    }
    virtual ~IKernel(){

    }
public:
    virtual bool connect(const char* szip = IP,short sport = PORT) = 0;
    virtual void disconnect(const char * szerr) = 0;
    virtual bool sendData(const char * szbuf,int nlen) = 0;
    virtual void dealData(const char * szbuf) = 0;
};

#endif // IKERNEL_H
