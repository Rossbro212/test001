
#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include "kernel/IKernel.h"
#define MAXNUM   1024


class TCPServer
{
public:
    TCPServer(IKernel *pKernel);
    ~TCPServer();
public:
    bool initNetWork(const char* szip = "127.0.0.1",short sport=1234);
    void unInitNetWork(const char* szerr);
    bool sendData(SOCKET sock,const char* szbuf,int nlen);
    void recvData();
public:
    static DWORD WINAPI ThreadProc(LPVOID lpvoid);
    static DWORD WINAPI ThreadRecv(LPVOID lpvoid);
public:
    SOCKET m_socklisten;
    std::list<HANDLE> m_lstThread;
    std::map<DWORD,SOCKET> m_mapThreadIdToSocket;
    bool  m_bflagQuit;
    IKernel *m_pKernel;
};

#endif // TCPNET_H


