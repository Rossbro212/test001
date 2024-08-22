#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "./kernel/tcpkernel.h"
#include <QDebug>
#include <QMessageBox>
#include "login.h"
#include <QFileDialog>
#include <QFile>
#include "md5.h"
#include <map>
#include <QDateTime>
#include <QTime>
#include "threadpool.h"
#include <QInputDialog>
//文件名---（文件路径，文件大小，文件id)
struct UploadFileInfo{
    char m_szFilePath[MAX_PATH];
    long long m_fileSize;
    long long m_fileId;
};
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
     void slot_login(STRU_LOGIN_RS* );
     void slot_getfilelist(STRU_GETFILELIST_RS* );
     void slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
     void slot_sharelink(STRU_SHARELLINK_RS*);
     void slot_getlink(STRU_GETLINK_RS*);
     void slot_html(QString);
 private slots:
     void on_pushButton_clicked();

     void on_pushButton_5_clicked();

     void on_pushButton_6_clicked();

 private:
    Ui::Widget *ui;

    login   *m_pLogin;
    long long m_userId;

    int m_nFileNum;
    Threadpool m_tp;
public:
    IKernel *m_pKernel;
    std::map<QString,UploadFileInfo*> m_mapFileNameToFileInfo;
};



class SendFileItask :public task{
public:
    SendFileItask(UploadFileInfo *pInfo,long long pos,long long fileId,Widget *pWidget){
        m_pInfo = pInfo;
        m_pos = pos;
        m_pWidget = pWidget;
        m_fileId = fileId;
    }
    ~SendFileItask(){

    }
public:
    void run();
private:
    UploadFileInfo *m_pInfo;
    long long m_pos;
    Widget *m_pWidget;
    long long m_fileId;
};
#endif // WIDGET_H
