#ifndef IKERNEL_H
#define IKERNEL_H

class IKernel {
public:
    IKernel(){

    }
    virtual ~IKernel(){

    }
public:
    virtual bool connect(const char* szip = "127.0.0.1",short sport=1234) = 0;
    virtual void disconnect(const char* szerr) = 0;
    virtual bool sendData(const char* szbuf,int nlen) = 0;
    virtual void dealData(const char* szbuf) = 0;
    virtual void dealHTML(const char* szbuf) = 0;
};

#endif // IKERNEL_H
