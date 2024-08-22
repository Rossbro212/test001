#include "login.h"
#include "ui_login.h"
#include <QDebug>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    setWindowTitle("disk");
    setWindowIcon(QIcon("./disk.JPG"));


}

login::~login()
{
    delete ui;
}

void login::slot_register(STRU_REGISTER_RS *psrr)
{
   const char *pszResult = "register_err";
    if(psrr->m_szResult == _register_success)
        pszResult = "register_success";

   QMessageBox::information(this,"register",pszResult);

}
//×¢²á
void login::on_pushButton_clicked()
{
    QString strTel = ui->lineEdit_rtel->text();
    QString strUser = ui->lineEdit_ruser->text();
    QString strPasswd = ui->lineEdit_rpasswd->text();

    STRU_REGISTER_RQ sgr;
    strcpy(sgr.m_szName,strUser.toStdString().c_str());
    strcpy(sgr.m_szPassword,strPasswd.toStdString().c_str());
    sgr.m_tel = strTel.toLongLong();

    m_pKernel->sendData((char*)&sgr,sizeof(sgr));

}

//µÇÂ¼
void login::on_pushButton_2_clicked()
{
    QString strUser = ui->lineEdit_luser->text();
    QString strPasswd = ui->lineEdit_lpasswd->text();

    STRU_LOGIN_RQ slr;
    strcpy(slr.m_szName,strUser.toStdString().c_str());
    strcpy(slr.m_szPassword,strPasswd.toStdString().c_str());

    m_pKernel->sendData((char*)&slr,sizeof(slr));
}

