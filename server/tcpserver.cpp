#include "tcpserver.h"

TCPServer::TCPServer(IKernel *pKernel)
{
    m_socklisten = 0;
    m_bflagQuit = true;
    m_pKernel = pKernel;
}

TCPServer::~TCPServer()
{

}

bool TCPServer::initNetWork(const char *szip, short sport)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
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

    m_socklisten = socket(AF_INET,SOCK_STREAM,0);
    if(INVALID_SOCKET == m_socklisten){
        unInitNetWork("socket err\n");
        return false;
    }
    sockaddr_in addrserver;
    addrserver.sin_family =AF_INET;
    addrserver.sin_addr.S_un.S_addr =0 /*inet_addr(szip)*/;
    addrserver.sin_port = htons(sport);
    if(SOCKET_ERROR ==bind(m_socklisten,(const sockaddr*)&addrserver,sizeof(addrserver))){
          unInitNetWork("bind err\n");
          return false;
    }

    if(SOCKET_ERROR ==listen(m_socklisten,128)){
         unInitNetWork("listen err\n");
         return false;
    }
    //创建线程---接收链接---
    HANDLE hThread = CreateThread(0,0,&ThreadProc,this,0,0);
    if(hThread)
         m_lstThread.push_back(hThread);
    return true;
}


DWORD TCPServer::ThreadProc(LPVOID lpvoid)
{
    TCPServer *pthis = (TCPServer*)lpvoid;
    sockaddr_in addrclient;
    int nsize = sizeof(sockaddr_in);

    DWORD dwThreadId = NULL;
    while(pthis->m_bflagQuit){
         //等待链接
         SOCKET sockwaiter = accept(pthis->m_socklisten,(struct sockaddr*)&addrclient,&nsize);
         printf("client ip:%s,port:%d\n",inet_ntoa(addrclient.sin_addr),ntohs(addrclient.sin_port));

         //创建线程
         HANDLE hThread =  CreateThread(0,0,&ThreadRecv,(LPVOID)pthis,0,&dwThreadId);
         if(hThread)
         {
             pthis->m_lstThread.push_back(hThread);
             pthis->m_mapThreadIdToSocket[dwThreadId] = sockwaiter;
         }
    }
    return 0;
}

DWORD TCPServer::ThreadRecv(LPVOID lpvoid)
{
    TCPServer *pthis = (TCPServer*)lpvoid;
    pthis->recvData();
    return 0;
}

void TCPServer::unInitNetWork(const char* szerr)
{
    m_bflagQuit = false;
    auto ite = m_lstThread.begin();
    while(ite != m_lstThread.end()){

         //判断线程是否退出
         if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
             TerminateThread(*ite,-1);

         CloseHandle(*ite);
         *ite= NULL;
         ite++;
    }
    m_lstThread.clear();

    printf(szerr);
    if(m_socklisten){

       closesocket(m_socklisten);
    }
    WSACleanup();



}

bool TCPServer::sendData(SOCKET sock, const char *szbuf, int nlen)
{
    if(sock == INVALID_SOCKET || !szbuf ||nlen <=0 )
       return false;
    //包大小
    if(send(sock,(char*)&nlen,sizeof(int),0)<=0)
       return false;

    //包内容
    if(send(sock,szbuf,nlen,0)<=0)
       return false;

    //序列化和反序列化--protobuf
    return true;
}

void TCPServer::recvData()
{
    SOCKET sockWaiter =  m_mapThreadIdToSocket[GetCurrentThreadId()];
    //怎么访问类成员

    int nReadNum;
    char szbuf[4096] = {0};
    while(m_bflagQuit){
       //接收数据
       //接收包大小

       nReadNum = recv(sockWaiter,szbuf,sizeof(szbuf),0);
       if(nReadNum >0){

            m_pKernel->dealHTML(szbuf);
       }




    }
}

