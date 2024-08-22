#include "widget.h"
#include "ui_widget.h"
#include <QIcon>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("disk");
    setWindowIcon(QIcon("./disk.JPG"));
    m_pKernel = new TCPKernel;
    m_pLogin = new login;
    m_pLogin->setKernel(m_pKernel);
    m_pLogin->show();
    if(m_pKernel->connect("127.0.0.1",1234)){
        qDebug()<<"connect success";
    }else{
        QMessageBox::information(this,"err","connect err");
    }
    m_nFileNum = 0;
    //创建线程池
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    m_tp.createThreadpool(1,si.dwNumberOfProcessors/(1-0.8));
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_register,m_pLogin,&login::slot_register,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_login,this,&Widget::slot_login,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_getfilelist,this,&Widget::slot_getfilelist,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_uploadfileinfo,this,&Widget::slot_uploadfileinfo,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_sharelink,this,&Widget::slot_sharelink,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_html,this,&Widget::slot_html,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_getlink,this,&Widget::slot_getlink,Qt::BlockingQueuedConnection);
}

Widget::~Widget()
{
    delete ui;
    delete m_pKernel;
    delete m_pLogin;
    m_tp.destroyThreadpool();
}

void Widget::slot_login(STRU_LOGIN_RS *pslr)
{
    const char *pszResult = "user noexist";
    if(pslr->m_szResult == _login_passwderr){
        pszResult = "password err";
    }else if(pslr->m_szResult == _login_success){
        //关闭登录窗口
        m_pLogin->hide();
        //显式主窗口
        this->show();
        m_userId = pslr->m_userId;
        //登录成功
        //获取文件列表的请求
        STRU_GETFILELIST_RQ sgr;
        sgr.m_userId = pslr->m_userId;
        m_pKernel->sendData((char*)&sgr,sizeof(sgr));
        return;
    }
    QMessageBox::information(this,"login",pszResult);
}

void Widget::slot_getfilelist(STRU_GETFILELIST_RS *psgr)
{
    m_nFileNum = psgr->m_FileNum;
    QTableWidgetItem *pitem;
    for(int i = 0; i < psgr->m_FileNum;i++){
        //name
        pitem = new QTableWidgetItem(QIcon("./file.JPG"),psgr->m_aryInfo[i].m_szFileName);
        ui->tableWidget->setItem(i,0,pitem);
        //size
        QString strSize = QString::number(psgr->m_aryInfo[i].m_fileSize);
        pitem = new QTableWidgetItem(strSize);
        ui->tableWidget->setItem(i,1,pitem);
        //uploadtime
        pitem = new QTableWidgetItem(psgr->m_aryInfo[i].m_szFileDateTime);
        ui->tableWidget->setItem(i,2,pitem);
    }
}


void SendFileItask::run()
{
    //发送文件
    FILE* pfile = fopen(m_pInfo->m_szFilePath,"rb");
    //如果是断点续传
    if(m_pos){
        //将文件指针移动到指定位置
        fseek(pfile,m_pos,SEEK_SET);
    }

    //正常发送文件内容
    STRU_UPLOADFILEBLOCK_RQ sur;
    sur.m_fileId = m_fileId;
    while(1){
        ssize_t nReadNum = fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),pfile);
        if(nReadNum >0){
            sur.m_fileNum = nReadNum;
            m_pWidget->m_pKernel->sendData((char*)&sur,sizeof(sur));
        }else{
            break;
        }
    }

    fclose(pfile);
    //将映射从map中移除
    QString strFilePath = m_pInfo->m_szFilePath;
    strFilePath = strFilePath.section('/',-1);
    auto ite = m_pWidget->m_mapFileNameToFileInfo.begin();
    while(ite != m_pWidget->m_mapFileNameToFileInfo.end()){
        if(ite->first == strFilePath){
            m_pWidget->m_mapFileNameToFileInfo.erase(ite);
            break;
        }
        ite++;
    }


}
void Widget::slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS *psur)
{
    QString strFileName = psur->m_szFileName;
    UploadFileInfo *pInfo =  m_mapFileNameToFileInfo[strFileName];
    //处理文件相关内容
    if(psur->m_szResult == _fileinfo_isuploaded){
        QMessageBox::information(this,"uploadfile","file is uploaded");
        return;
    }else if(psur->m_szResult == _fileinfo_speedtransfer){
        //文件已经上传成功

    }else if(psur->m_szResult == _fileinfo_normal || psur->m_szResult == _fileinfo_continue){

        //将任务投递到线程池中
        task *p = new SendFileItask(pInfo,psur->m_pos,psur->m_fileId,this);
        m_tp.push(p);

    }

    //1.将文件信息，显示到控件上
    QTableWidgetItem *p = new QTableWidgetItem(QIcon("./file.JPG"),strFileName);
    ui->tableWidget->setItem(m_nFileNum,0,p);
    QString strFileSize = QString::number(pInfo->m_fileSize);
    p = new QTableWidgetItem(strFileSize);
    ui->tableWidget->setItem(m_nFileNum,1,p);
    QDateTime datetime = QDateTime::currentDateTime();
    QTime     time =QTime::currentTime();
   QString strDate = datetime.toString("dd.MM.yyyy");
   QString strTime = time.toString("HH:mm:ss");
   p = new QTableWidgetItem(strDate +" "+strTime );
   ui->tableWidget->setItem(m_nFileNum,2,p);
   ++m_nFileNum;
    //2.提示文件上传成功
   QMessageBox::information(this,"uploadfile","file upload success");
}

void Widget::slot_sharelink(STRU_SHARELLINK_RS *pssr)
{
   QString strLink = pssr->m_szLink;
   strLink = strLink + " code: " + pssr->m_szCode;

   QInputDialog::getText(this, tr("share link"),
                                        tr("share link address:"), QLineEdit::Normal,
                                        strLink);


}

void Widget::slot_getlink(STRU_GETLINK_RS *psgr)
{
   if(psgr->m_userId == m_userId){
        QMessageBox::information(this,"get link","file is yourself");
        return;
   }else{

        QTableWidgetItem *p = new QTableWidgetItem(QIcon("./file.JPG"),psgr->m_szFileName);
        ui->tableWidget->setItem(m_nFileNum,0,p);
        QString strFileSize = QString::number(psgr->m_FileSize);
        p = new QTableWidgetItem(strFileSize);
        ui->tableWidget->setItem(m_nFileNum,1,p);

        p = new QTableWidgetItem(psgr->m_szFileUploadTime );
        ui->tableWidget->setItem(m_nFileNum,2,p);
        ++m_nFileNum;
         QMessageBox::information(this,"get link","success");
   }
}

void Widget::slot_html(QString strCode)
{
  //向服务器发送提取链接的请求
   STRU_GETLINK_RQ sgr;
   sgr.m_userId = m_userId;
   strcpy(sgr.m_szCode,strCode.toStdString().c_str());
   m_pKernel->sendData((char*)&sgr,sizeof(sgr));

}
string FileDigest(QString filename) {

    QFile file(filename);

    file.open(QIODevice::ReadOnly);
    if (!file.isOpen())
        return "";

    MD5 md5;
    char buffer[1024];
    while (1)
    {
        qint64 length =  file.read(buffer, 1024);

        if (length > 0)
            md5.update(buffer, length);
        else
            break;
    }
    file.close();
    return md5.toString();
}
void Widget::on_pushButton_clicked()
{
    //获取要上传的文件信息（文件名、文件大小、文件MD5）
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    ".",//打开文件夹的根目录
                                                    "ALL files (*.*);;Images (*.png *.xpm *.jpg);;Text files (*.txt)"
                                                    );

    QString fileName = filePath.section('/',-1);
    long long filesize;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)){

       filesize = file.size();
       file.close();
    }


    string strMD5 = FileDigest(filePath);
    qDebug()<<strMD5.c_str();
    //上传文件信息请求
    STRU_UPLOADFILEINFO_RQ sur;
    sur.m_filesize = filesize;
    sur.m_userId = m_userId;
    strcpy(sur.m_szFileMD5,strMD5.c_str());
    strcpy(sur.m_szFileName,fileName.toStdString().c_str());

    m_pKernel->sendData((char*)&sur,sizeof(sur));

    //记录当前文件信息、文件名---（文件路径，文件大小，文件id)
    UploadFileInfo *p = new UploadFileInfo;
    strcpy(p->m_szFilePath,filePath.toStdString().c_str());
    p->m_fileId = 0;
    p->m_fileSize = filesize;

    m_mapFileNameToFileInfo[fileName] = p;

}


void Widget::on_pushButton_5_clicked()
{
    //获取要分享的文件
    int nRow =  ui->tableWidget->currentRow();
    if(nRow == -1)return;
    QTableWidgetItem *pItem = ui->tableWidget->item(nRow,0);

    //发送分享链接请求
    STRU_SHARELINK_RQ ssr;
    ssr.m_userId = m_userId;
    strcpy(ssr.m_szFileName,pItem->text().toStdString().c_str());

    m_pKernel->sendData((char*)&ssr,sizeof(ssr));
}


void Widget::on_pushButton_6_clicked()
{
    QString strCode = ui->lineEdit->text();
    //向服务器发送提取链接的请求
    STRU_GETLINK_RQ sgr;
    sgr.m_userId = m_userId;
    strcpy(sgr.m_szCode,strCode.toStdString().c_str());
    m_pKernel->sendData((char*)&sgr,sizeof(sgr));
}

