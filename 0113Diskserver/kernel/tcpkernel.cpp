#include "tcpkernel.h"
#include"winsock2.h"
#include"../server/tcpnet.h"
tcpkernel::tcpkernel() {
     m_pTCPNet=new TCPNet;
     m_cMysql=new CMySql;
     strcpy(m_szSystemPath,"E:/SQLLearn/disk/");
}
//tcpkernel *tcpkernel::m_pKernel=NULL;
tcpkernel *tcpkernel::m_pKernel=new tcpkernel;
tcpkernel::~tcpkernel()
{
    delete m_pTCPNet;
    delete m_cMysql;
}

bool tcpkernel::open()
{
    if(!m_pTCPNet->initNetWork())
    {
        printf("init network err");
        return false;
    }
    if(!m_cMysql->ConnectMySql("localhost","root","123456","0113disk"))
    {
        printf("mysql connect err");
        return false;
    }
    return true;
}

void tcpkernel::close()
{
    m_pTCPNet->unInitNetWork("err");
    m_cMysql->DisConnect();
}

void tcpkernel::dealdata(SOCKET sock, char *szbuf)
{
    switch(*szbuf)
    {
    case _default_protocol_register_rq:
    {
        registerrq(sock,szbuf);
        break;
    }
    case _default_protocol_login_rq:
    {
        loginrq(sock,szbuf);
        break;
    }
    case _default_protocol_getfilelist_rq:
    {
        getfilelistrq(sock,szbuf);
        break;
    }
    case _default_protocol_uploadfileinfo_rq:
    {
        uploadfileinforq(sock,szbuf);
        break;
    }
    case _default_protocol_uploadfileblock_rq:
    {
        uploadfileblockrq(sock,szbuf);
        break;
    }
    case _default_protocol_sharelink_rq:
    {
        sharelinkrq(sock,szbuf);
        break;
    }
    case _default_protocol_getlink_rq:
    {
        getlinkrq(sock,szbuf);
        break;
    }
    case _default_protocol_deletefile_rq:
        deleteFilerq(sock,szbuf);
        break;
    case _default_protocol_download_rq:
        downloadrq(sock,szbuf);
        break;
    case _default_protocol_searchfile_rq:
        searchfilerq(sock,szbuf);
        break;
    case  _default_protocol_downloadblock_rq:
        downloadblock(sock,szbuf);
        break;
    }
}
 void tcpkernel::registerrq(SOCKET sock,char* szbuf)
{
       //将数据插入到数据库中
    STRU_REGISTER_RQ *psrr=(STRU_REGISTER_RQ*)szbuf;
    char szsql[SQLLEN]={0};
    char szPath[MAX_PATH]={0};

    list<string> lststr;
    STRU_REGISTER_RS srr;
       srr.m_szResult=_register_err;
    sprintf(szsql,"insert into user(u_name,u_password,u_tel) values('%s','%s','%lld')",
              psrr->m_szName,psrr->m_szPassword,psrr->m_tel);
       if(m_cMysql->UpdateMySql(szsql))
    {
        srr.m_szResult=_register_success;
         sprintf(szsql,"select u_id from user where u_tel=%lld",psrr->m_tel);
        m_cMysql->SelectMySql(szsql,1,lststr);
           if(lststr.size()>0)
        {
            string strUserId=lststr.front();
               lststr.pop_front();
            //为当前用户创建 目录
               sprintf(szPath,"%s%s",m_szSystemPath,strUserId.c_str());
               CreateDirectoryA(szPath,0);
        }
    }
       m_pTCPNet->sendData(sock,(char*)&srr,sizeof(srr));
 }
 void tcpkernel::loginrq(SOCKET sock,char* szbuf)
 {
     STRU_LOGIN_RQ *pslr= (STRU_LOGIN_RQ*)szbuf;
     //校验用户名 是否存在
     char szsql[SQLLEN]={0};
     list<string> lstStr;
     STRU_LOGIN_RS srr;
     srr.m_szResult= _login_usernoexist; //默认用户名不存在
     sprintf(szsql,"select u_id,u_password from user where u_name='%s'\n",pslr->m_szName); //查询数据库 检验用户名是否存在
     m_cMysql->SelectMySql(szsql,2,lstStr);

     if(lstStr.size()>0)
     {
         srr.m_szResult= _login_passworderr; //默认密码错误
         string strUserId=lstStr.front();   //先是uid
         lstStr.pop_front();
         string strPassword=lstStr.front();  //后是密码
         lstStr.pop_front();
         if(0==strcmp(pslr->m_szPassword,strPassword.c_str()))
         {
             srr.m_szResult=_login_success;  //成功

             srr.m_userId=atoll(strUserId.c_str());
             printf("login success！！！\n");
         }
     }
        m_pTCPNet->sendData(sock,(char*)&srr,sizeof(srr));

 }

 void tcpkernel::getfilelistrq(SOCKET sock, char *szbuf) //获取文件列表
 {
     STRU_GETFILELIST_RQ *gflq=(STRU_GETFILELIST_RQ*)szbuf;
     char  szsql[SQLLEN]={0};
     STRU_GETFILELIST_RS sgr;
     sprintf(szsql,"select f_name,f_size,f_uploadtime from myview where u_id= %lld",gflq->m_userId);
     list<string> lststr;
     m_cMysql->SelectMySql(szsql,3,lststr);
     int i=0;
     while(lststr.size()>0)
     {
         string strfileName=lststr.front();
         lststr.pop_front();
         string strfilesize=lststr.front();
         lststr.pop_front();
         string strfileuploadtime=lststr.front();
         lststr.pop_front();
         strcpy(sgr.m_aryInfo[i].m_szFileName,strfileName.c_str());
         strcpy(sgr.m_aryInfo[i].m_szFileDateTime,strfileuploadtime.c_str());
         sgr.m_aryInfo[i].m_fileSize=atoll(strfilesize.c_str());
         i++;
         if(i==FILENUM||lststr.size()==0)
         {
             sgr.m_FileNum=i;  //包含的文件大小
             m_pTCPNet->sendData(sock,(char*)&sgr,sizeof(sgr));
             ZeroMemory(sgr.m_aryInfo,sizeof(sgr.m_aryInfo));
             i=0;
         }
     }
 }

 void tcpkernel::uploadfileinforq(SOCKET sock, char *szbuf)
 {
     STRU_UPLOADFILEINFO_RQ* psur=(STRU_UPLOADFILEINFO_RQ*)szbuf;

     //2.1将文件信息加入到数据库中
     //2.2 将文件与用户信息 映射到user_file
     //2.3创建新文件
     STRU_UPLOADFILEINFO_RS sur;

     strcpy(sur.m_szFileName,psur->m_szFileName);
     sur.m_pos=0;
     char szsql[SQLLEN]={0};
     list<string> lststr;
    sprintf(szsql,
             "select u_id,f_id,f_count from myview where f_md5='%s';",
             psur->m_szFileMD5);
    // sprintf(szsql,"select distinct u_id,file.* from user_file inner join  file on  f_md5='%s'",psur->m_szFileMD5);
     m_cMysql->SelectMySql(szsql,3,lststr);
     if(lststr.size()>0)
     {
         //1.判断数据库中是否存在热门文件
         string strUserId=lststr.front(); //用户ID
         lststr.pop_front();
         string strFileId=lststr.front();//文件ID
         lststr.pop_front();
         string strFilecount=lststr.front();//文件引用计数
         lststr.pop_front();
         long long userId=atoll(strUserId.c_str());
         long long FileId=atoll(strFileId.c_str());
         sur.m_fileId=FileId;
         if(psur->m_userId==userId) //校验用户--是同一用户 自己传过
         {//1.1判断这个文件是不是自己传过的，但是1.没有传完---断点续传
             sur.m_szResult=_fileinfo_isuploaded;
             uploadfileinfo* pInfo= m_mapfileIdtoFileInfo[FileId];
             if(pInfo)
             {
                 sur.m_szResult=_fileinfo_continue;
                 sur.m_pos =pInfo->m_pos;
             } //2.传完了---重复上传
         }else // 不是该用户
         {
             sur.m_szResult=_fileinfo_speedtransfer;
             sprintf(szsql,"update file set f_count =f_count+1 where f_id=%lld; ",FileId); //文件引用计数加1
             m_cMysql->UpdateMySql(szsql);
             //1.2如果是文件是别人传过的，存在则秒传成功--文件引用计数+1
             sprintf(szsql,"insert into user_file(u_id,f_id) values(%lld,%lld)",psur->m_userId,FileId);
             m_cMysql->UpdateMySql(szsql);
              //2.2 将文件与用户信息 映射到user_file
         }
     }
     else
      {     //2.数据库不存在
         sur.m_szResult=_fileinfo_normal;
         char szFilePath[MAX_PATH]={0};
         sprintf(szFilePath,"%s%lld/%s",m_szSystemPath,psur->m_userId,psur->m_szFileName);
         sprintf(szsql,
                 "insert into file(f_name,f_size,f_path,f_md5) values('%s',%lld,'%s','%s') "
                 ,psur->m_szFileName,psur->m_filesize,szFilePath,psur->m_szFileMD5);

         //2.1将文件信息加入到数据库中
         m_cMysql->UpdateMySql(szsql);

         //2.2 将文件与用户信息 映射到user_file
         sprintf(szsql,"select f_id from file where f_md5='%s'",psur->m_szFileMD5);
         m_cMysql->SelectMySql(szsql,1,lststr);
         if(lststr.size() >0)
         {
             string strFileId = lststr.front();
             lststr.pop_front();
             sprintf(szsql,"insert into user_file(u_id,f_id) values(%lld,%lld);",psur->m_userId,atoll(strFileId.c_str()));
             m_cMysql->UpdateMySql(szsql);
         }
         //2.3创建新文件
         FILE* pfile=fopen(szFilePath,"wb");
         //记录文件信息  文件ID-- 文件指针  userid  文件大小 文件位置
         uploadfileinfo *p=new uploadfileinfo;
         p->m_pFile=pfile;
         p->m_pos=0;
         p->m_pFileSize=psur->m_filesize;
         p->m_userId=psur->m_userId;
         m_mapfileIdtoFileInfo[sur.m_fileId] = p;
     }
     m_pTCPNet->sendData(sock,(char*)&sur,sizeof(sur));
 }

 void tcpkernel::uploadfileblockrq(SOCKET sock, char *szbuf)
 {
   //将文件内容 写入对应的文件中
     STRU_UPLOADFILEBLOCK_RQ* psur=(STRU_UPLOADFILEBLOCK_RQ*)szbuf;

      uploadfileinfo* p=m_mapfileIdtoFileInfo[psur->m_fileId];
     if(!p)
       return;
     ssize_t nWriteNum=fwrite(psur->m_szFileContent,sizeof(char),psur->m_fileNum,p->m_pFile);
     if(nWriteNum>0)
     {
         p->m_pos+=nWriteNum;
         if(p->m_pos==p->m_pFileSize)
         {
             fclose(p->m_pFile);
         auto ite =m_mapfileIdtoFileInfo.begin();
         while(ite!=m_mapfileIdtoFileInfo.end())
         {
             if(ite->first==psur->m_fileId)
             {
                 m_mapfileIdtoFileInfo.erase(ite);
                 break;
              }
             ite++;
           }
        }
     }
 }

 void tcpkernel::sharelinkrq(SOCKET sock, char *szbuf)
 {
     STRU_SHARELINK_RQ* pssr=(STRU_SHARELINK_RQ*)szbuf;
     STRU_SHARELLINK_RS srr;
     strcpy(srr.m_szFileName,pssr->m_szFileName);
     srand((unsigned int)time(0));
     char c;
     char szCode[MAXSIZE]={0};
     for (int i = 0; i < 4; i++) {
         int randNum = rand() % 36;
         if (randNum < 10){
             c = randNum + '0'; // 生成数字 0-9 对应的 ASCII 字符
         }
         else {
             c = randNum - 10 + 'A'; // 生成字母 A-Z 对应的 ASCII 字符
         }
         szCode[i]= c;
     }
     char szsql[SQLLEN]={0};
     list<string> lststr;
     sprintf(szsql,"select f_id from myview where u_id= %lld and f_name = '%s'",
             pssr->m_userId,pssr->m_szFileName);
     m_cMysql->SelectMySql(szsql,1,lststr);
     if(lststr.size()>0)
     {
         string strFileId=lststr.front();
         lststr.pop_front();
         long long FileId=atoll(strFileId.c_str());
         //将文件分享到数据库中
         sprintf(szsql,"insert into user_share(u_id,f_id,code) values(%lld,%lld,'%s')",
                 pssr->m_userId,FileId,szCode);
         m_cMysql->UpdateMySql(szsql);
     }
     strcpy(srr.m_szCode,szCode);
     strcpy(srr.m_szLink,"E:/SQLLearn/disk/23 \n");

     m_pTCPNet->sendData(sock,(char*)&srr,sizeof(srr));
 }

 void tcpkernel::getlinkrq(SOCKET sock, char *szbuf)
 {
     //1.通过提取码  找到对应的user_ID,以及fileid
     //2.通过fileid, filename filesize  fileuploadtime
     //3.将文件引用计数加1
     //4.发送回复
     STRU_GETLINK_RQ *psgr=(STRU_GETLINK_RQ*)szbuf;
     STRU_GETLINK_RS sgr;
     char szsql[SQLLEN]={0};
     list<string> lststr;
     sprintf(szsql,"select u_id,f_id from user_share where code = '%s';",psgr->m_szCode);
     m_cMysql->SelectMySql(szsql,2,lststr);
     if(lststr.size() >0){
         string strUserId = lststr.front();
         lststr.pop_front();
         string strFileId = lststr.front();
         lststr.pop_front();
         long long userId = atoll(strUserId.c_str());
         long long FileId = atoll(strFileId.c_str());
           //2.通过FILEID--filename,filesize,fileuploadtime
         sprintf(szsql,"select f_name,f_size,f_uploadtime from file where f_id = %lld;",FileId);
         m_cMysql->SelectMySql(szsql,3,lststr);
         if(lststr.size()>0){
                  string strFileName = lststr.front();
                 lststr.pop_front();
                  string strFileSize = lststr.front();
                 lststr.pop_front();
                  string strFileUploadTime = lststr.front();
                 lststr.pop_front();
                  strcpy(sgr.m_szFileName,strFileName.c_str());
                 strcpy(sgr.m_szFileUploadTime,strFileUploadTime.c_str());
                  sgr.m_FileSize=atoll(strFileSize.c_str());
                 sgr.m_userId=userId;
                             }
         sprintf(szsql,"update from file set f_count =f_count+1 where f_id=%lld",FileId);
         m_cMysql->UpdateMySql(szsql);
     }
     m_pTCPNet->sendData(sock,(char*)&sgr,sizeof(sgr));
 }
 void tcpkernel::deleteFilerq(SOCKET sock, char *szbuf)//(ps:)删除文件后该文件存在于文件列表中
                                                    //关掉重新开客户端之后才可以在列表中不显示
 {
     STRU_DELETEFILE_RQ* psdr=(STRU_DELETEFILE_RQ*)szbuf;
     STRU_DELETEFILE_RS pdrr;
     pdrr.m_szResult=_login_deleteErr;
     char szsql[SQLLEN]={0};
     char szFilePath[MAX_PATH]={0};
     sprintf(szFilePath,"%s%lld/%s",m_szSystemPath,psdr->m_userId,psdr->m_fileName);
     list<string> lststr;
     sprintf(szsql,"select f_id from file where f_name= '%s'"
                    " and f_size= %lld and f_uploadtime = '%s';"
             ,psdr->m_fileName,psdr->m_FileSize,psdr->m_szFileUploadTime);
     m_cMysql->SelectMySql(szsql,1,lststr);
     if(lststr.size()>0)
     {
     sprintf(szsql,"delete from file where f_name= '%s' and f_size= %lld and f_uploadtime = '%s';"
     ,psdr->m_fileName,psdr->m_FileSize,psdr->m_szFileUploadTime);
     m_cMysql->UpdateMySql(szsql);
     pdrr.m_szResult=_login_deleteSuccess;
     QFile file(szFilePath);//在文件路径中删除
     if (file.exists()) {
         if (file.remove()) {
             m_pTCPNet->sendData(sock, (char*)&pdrr, sizeof(pdrr));
         }
     }
    }
 }
 void tcpkernel::downloadrq(SOCKET sock, char *szbuf)
 {
     STRU_download_RQ* pddr=(STRU_download_RQ*)szbuf;
     STRU_download_RS pdrg;
     pdrg.m_szResult=_downloadErr;
     char szsql[SQLLEN]={0};
     char szFilePath[MAX_PATH]={0};
     list<string> lststr;
     sprintf(szFilePath,"%s%lld/%s",m_szSystemPath,pddr->m_userId,pddr->m_szFileName);
     sprintf(szsql,
             "insert into file(f_name,f_size,f_path,f_md5) values('%s',%lld,'%s','%s') "
             ,pddr->m_szFileName,pddr->m_filesize,szFilePath,pddr->m_szFileMD5);
     //将文件信息加入到数据库中
     m_cMysql->UpdateMySql(szsql);
     //将文件信息映射到user——file表中
     sprintf(szsql,"select f_id from file where f_md5='%s'",pddr->m_szFileMD5);
     m_cMysql->SelectMySql(szsql,1,lststr);
     if(lststr.size() >0)
     {
         string strFileId = lststr.front();
         lststr.pop_front();
         sprintf(szsql,"insert into user_file(u_id,f_id) values(%lld,%lld);",pddr->m_userId,atoll(strFileId.c_str()));
         m_cMysql->UpdateMySql(szsql);
         pdrg.m_szResult=_downloadSuccess;
     }
     strcpy(pdrg.m_szFileName,pddr->m_szFileName);
     pdrg.m_filesize=pddr->m_filesize;
     FILE* pfile=fopen(szFilePath,"wb");
     //记录文件信息  文件ID-- 文件指针  userid  文件大小 文件位置
     downloadinfo *p=new downloadinfo;
     p->m_pFile=pfile;
     p->m_pos=0;
     p->m_pFileSize=pddr->m_filesize;
     p->m_userId=pddr->m_userId;
     m_mapfIdtoFileInfo[pdrg.m_fileId] = p;
     m_pTCPNet->sendData(sock,(char*)&pdrg,sizeof(pdrg));
 }

 void tcpkernel::searchfilerq(SOCKET sock, char *szbuf)
 {
     STRU_SEARCHFILE_RQ* pssr=(STRU_SEARCHFILE_RQ*)szbuf;
     STRU_SEARCHFILE_RS pssq;
    // searchfileinfo* p=m_mapfileNametoFileInfo[pssr->m_szFileName];
   // if(!p)
    //  return;
     pssq.m_szResult=_searchfile_Err;//默认失败
     char szsql[SQLLEN]={0};
     list<string> lststr;
     long long nNum=0;
     sprintf(szsql,"select f_name,f_size,f_uploadtime from  file  left join user_file on user_file.f_id = file.f_id and u_id= '%d' where f_name like '%c%s%c';",
                        pssr->m_userId,'%',pssr->m_szFileName,'%');
     m_cMysql->SelectMySql(szsql,3,lststr);
    // while(!lststr.empty())
     //{
     for(int i=0;i<lststr.size();i++){
         nNum++;
         string strfileName=lststr.front();
         lststr.pop_front();
         string strfilesize=lststr.front();
         lststr.pop_front();
         string strfileuploadtime=lststr.front();
         lststr.pop_front();
         strcpy(pssq.m_searchfileInfo[i].m_szFileName,strfileName.c_str());
         strcpy(pssq.m_searchfileInfo[i].m_szFileDateTime,strfileuploadtime.c_str());
         pssq.m_searchfileInfo[i].m_fileSize=atoll(strfilesize.c_str());
         pssq.m_szResult=_searchfile_Success;
     }
   //  }
     pssq.filelength=nNum;
     m_pTCPNet->sendData(sock,(char*)&pssq,sizeof(pssq));
 }

 void tcpkernel::downloadblock(SOCKET sock, char *szbuf)
 {
     STRU_downloadblock_RQ* psdr=(STRU_downloadblock_RQ*)szbuf;
     downloadinfo* p=m_mapfIdtoFileInfo[psdr->m_fileId];
     if(!p)
         return;
     ssize_t nWriteNum=fwrite(psdr->m_szFileContent,sizeof(char),psdr->m_fileNum,p->m_pFile);
     if(nWriteNum>0)
     {
         p->m_pos+=nWriteNum;
         if(p->m_pos==p->m_pFileSize)
         {
             fclose(p->m_pFile);
             auto ite =m_mapfIdtoFileInfo.begin();
             while(ite!=m_mapfIdtoFileInfo.end())
             {
                 if(ite->first==psdr->m_fileId)
                 {
                     m_mapfIdtoFileInfo.erase(ite);
                     break;
                 }
                 ite++;
             }
         }
     }
 }

