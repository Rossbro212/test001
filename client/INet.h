#ifndef INET_H
#define INET_H

class INet
{
public:
    INet(){

    }
    virtual ~INet(){

    }
public:
   virtual bool connectServer(const char* szip = "127.0.0.1",short sport=1234) = 0;
   virtual void disconnectServer(const char* szerr) = 0;
   virtual bool sendData(const char* szbuf,int nlen) = 0;
   virtual void recvData() = 0;

};

#endif // INET_H
