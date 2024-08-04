#ifndef TCPKERNEL_H
#define TCPKERNEL_H
#include"MySql/CMySql.h"
#include"IKernel.h"
#include"../server/tcpnet.h"
#include"../server/INet.h"
#include"Packdef.h"
#include<map>
#include<time.h>
#include<QFile>
#include<QDatetime>
#include<QTime>
struct uploadfileinfo{
    FILE* m_pFile;
    long long m_pFileSize;
    long long m_pos;
    long long m_userId;
};
struct downloadinfo{
    FILE* m_pFile;
    long long m_pFileSize;
    long long m_pos;
    long long m_userId;
};
struct searchfileinfo{
    FILE* m_pFile;
    long long m_pFileSize;
    char f_uploadtime;
};
class tcpkernel :public IKernel
{
private:
    tcpkernel();
    ~tcpkernel();
public:
    virtual bool open();
    virtual void close();
    virtual void dealdata(SOCKET sock,char* szbuf);
    //懒汉模式  不支持线程安全
   //static IKernel* getKernel()
   // {
   //     if(!m_pKernel)
   //     {
   //         m_pKernel=new tcpkernel;
   //     }
   //     return m_pKernel;
   // }
    //饿汉模式 支持
    static IKernel* getKernel()
    {
        return m_pKernel;
    }
public:
    void registerrq(SOCKET sock,char* szbuf);
     void loginrq(SOCKET sock,char* szbuf);
    void getfilelistrq(SOCKET sock,char* szbuf);
     void uploadfileinforq(SOCKET sock,char* szbuf);
    void uploadfileblockrq(SOCKET sock,char* szbuf);
     void sharelinkrq(SOCKET sock,char* szbuf);
    void getlinkrq(SOCKET sock,char* szbuf);
     void deleteFilerq(SOCKET sock,char* szbuf);
    void downloadrq(SOCKET sock,char* szbuf);
     void searchfilerq(SOCKET sock,char* szbuf);
    void downloadblock(SOCKET sock,char* szbuf);
     //E:\SQLLearn\disk
private:
    INet* m_pTCPNet;
    CMySql* m_cMysql;
    static tcpkernel *m_pKernel;//静态对象
    char m_szSystemPath[MAX_PATH];
    std::map<long long,uploadfileinfo*> m_mapfileIdtoFileInfo;
    std::map<long long,downloadinfo*> m_mapfIdtoFileInfo;
    std::map<long long,searchfileinfo*> m_mapfileNametoFileInfo;
};

#endif // TCPKERNEL_H
