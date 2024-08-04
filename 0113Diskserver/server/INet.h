#ifndef INET_H
#define INET_H
#include "server/tcpnet.h"
#include<winsock2.h>
class INet
{
public:
    INet()
    {

    }
   ~INet()
    {

    }
     virtual bool initNetWork(const char* szip = "192.168.71.1",short sport=1234) = 0;
     virtual void unInitNetWork(const char* szerr) = 0;
     virtual bool sendData(SOCKET sock,const char* szbuf,int nlen) = 0;
     virtual void recvData() = 0;

};
#endif // INET_H
