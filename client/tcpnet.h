#ifndef TCPNET_H
#define TCPNET_H
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXNUM   1024
#include "INet.h"
#include "kernel/IKernel.h"
class TCPNet :public INet
{
public:
    TCPNet(IKernel * pkernel);
    ~TCPNet();
public:
    bool connectServer(const char* szip = "127.0.0.1",short sport=1234);
    void disconnectServer(const char* szerr);
    bool sendData(const char* szbuf,int nlen);
    void recvData();
    static DWORD WINAPI ThreadProc(LPVOID lpvoid);
private:
    SOCKET m_sockclient;
    HANDLE m_hThread;
    bool   m_bflagQuit;
    IKernel *m_pKernel;
};

#endif // TCPNET_H
