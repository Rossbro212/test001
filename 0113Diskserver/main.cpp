#include <QCoreApplication>
#include "./kernel/tcpkernel.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
     IKernel *p =tcpkernel::getKernel();
    if(p->open())
    {
        printf("SERVER IS running\n");
    }else
    {
        printf("server err\n");
    }
   //STRU_REGISTER_RQ srr;
   //strcpy(srr.m_szName,"xiaojindou");
   //strcpy(srr.m_szPassword,"123321");
   //srr.m_tel=12345678906;
   //p->dealdata(0,(char*)&srr);

   //STRU_LOGIN_RQ srr1;
   //strcpy(srr1.m_szName,"xiaojindou");
   //strcpy(srr1.m_szPassword,"123321");
   //p->dealdata(0,(char*)&srr1);
  // STRU_GETFILELIST_RQ srr2;
  // srr2.m_userId=1;
  // p->dealdata(0,(char*)&srr2);
    return a.exec();
}
