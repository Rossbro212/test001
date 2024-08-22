#include "tcpkernel.h"

TCPKernel::TCPKernel(QObject *parent)
    : QObject{parent}
{
    m_pTCPNet  = new TCPNet(this);
    m_pServer = new TCPServer(this);
}

TCPKernel::~TCPKernel()
{
    delete m_pTCPNet;
    delete m_pServer;
}

bool TCPKernel::connect(const char *szip, short sport)
{
    if(!m_pTCPNet->connectServer(szip,sport))
        return false;
    if(!m_pServer->initNetWork("127.0.0.1",8899))
        return false;
    return true;
}

void TCPKernel::disconnect(const char *szerr)
{
    m_pTCPNet->disconnectServer(szerr);
}

bool TCPKernel::sendData(const char *szbuf, int nlen)
{
    if(!m_pTCPNet->sendData(szbuf,nlen))
        return false;

    return true;
}

void TCPKernel::dealData(const char *szbuf)
{
   //todo
    switch(*szbuf){
    case _default_protocol_register_rs:
        emit signal_register((STRU_REGISTER_RS*)szbuf);
        break;
    case _default_protocol_login_rs:
        emit signal_login((STRU_LOGIN_RS*)szbuf);
        break;
    case _default_protocol_getfilelist_rs:
        emit signal_getfilelist((STRU_GETFILELIST_RS*)szbuf);
        break;
    case _default_protocol_uploadfileinfo_rs:
        emit signal_uploadfileinfo((STRU_UPLOADFILEINFO_RS*)szbuf);
        break;

    case _default_protocol_getlink_rs:
        emit signal_getlink((STRU_GETLINK_RS*)szbuf);
        break;
    }
}

void TCPKernel::dealHTML(const char *szbuf)
{
    //html 响应头
    QString str = szbuf;
    str = str.section("text=",-1);//从后向前找到字符串

    emit signal_html(str);
}
