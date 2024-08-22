#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include <QObject>
#include "IKernel.h"
#include "../client/tcpnet.h"
#include "../server/tcpserver.h"
#include "Packdef.h"
class TCPKernel : public QObject,public IKernel
{
    Q_OBJECT
public:
    explicit TCPKernel(QObject *parent = nullptr);
    ~TCPKernel();
public:
    bool connect(const char* szip = "127.0.0.1",short sport=1234);
    void disconnect(const char* szerr);
    bool sendData(const char* szbuf,int nlen);
    void dealData(const char* szbuf);
    void dealHTML(const char* szbuf);
signals:
    void signal_register(STRU_REGISTER_RS* );
    void signal_login(STRU_LOGIN_RS* );
    void signal_getfilelist(STRU_GETFILELIST_RS* );
    void signal_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
    void signal_sharelink(STRU_SHARELLINK_RS*);
    void signal_getlink(STRU_GETLINK_RS*);
    void signal_html(QString);
private:
    INet *m_pTCPNet;
    TCPServer *m_pServer;
};

#endif // TCPKERNEL_H
