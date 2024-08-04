#ifndef PACKDEF_H
#define PACKDEF_H
#define _default_protocol_base   10
//申请账号
#define _default_protocol_register_rq   _default_protocol_base +1  //登录的请求和回复
#define _default_protocol_register_rs   _default_protocol_base +2
//登陆
#define _default_protocol_login_rq   _default_protocol_base +3
#define _default_protocol_login_rs   _default_protocol_base +4
//获取文件列表
#define _default_protocol_getfilelist_rq   _default_protocol_base +5
#define _default_protocol_getfilelist_rs   _default_protocol_base +6
//上传文件---正常传文件、断点续传、秒传
#define _default_protocol_uploadfileinfo_rq   _default_protocol_base +7
#define _default_protocol_uploadfileinfo_rs   _default_protocol_base +8
#define _default_protocol_uploadfileblock_rq   _default_protocol_base +9
#define _default_protocol_uploadfileblock_rs   _default_protocol_base +10

//搜索文件

//删除文件
#define _default_protocol_deletefile_rq   _default_protocol_base +13
#define _default_protocol_deletefile_rs   _default_protocol_base +14
//下载文件

//分享
//提取

//添加好友
//搜索好友
//删除好友
//聊天

//协议包

#define MAXSIZE  45
#define FILENUM  15
#define SQLLEN   300
#define _register_success    1
#define _register_err        0
#define _login_usernoexist   0
#define _login_passworderr   1
#define _login_success       2
#define ONE_PAGE 40
#define _fileinfo_isuploaded   0    //文件已经上传过了，重复上传
#define _fileinfo_continue     1     //断点续传2
#define _fileinfo_speedtransfer  2  //秒传
#define _fileinfo_normal       3    //正常
struct STRU_BASE{
    char m_nType;//包类型
};
#define _default_protocol_sharelink_rq   _default_protocol_base +15
#define _default_protocol_sharelink_rs   _default_protocol_base +16
//提取
#define _default_protocol_getlink_rq   _default_protocol_base +17
#define _default_protocol_getlink_rs   _default_protocol_base +18



#define _default_protocol_download_rq   _default_protocol_base +19
#define _default_protocol_download_rs   _default_protocol_base +20
#define _default_protocol_downloadblock_rq   _default_protocol_base +24
//新增功能 下载
#define _downloadSuccess       0  //下载成功
#define _downloadErr           1  //下载失败
struct STRU_download_RQ:public STRU_BASE{
    STRU_download_RQ(){
        m_nType = _default_protocol_download_rq;
    }
    char m_szFileUploadTime[MAXSIZE];
    long long m_userId;
    char m_szFileName[MAXSIZE];
    long long m_filesize;
    char m_szFileMD5[MAXSIZE];

};
struct STRU_download_RS:public STRU_BASE{
    STRU_download_RS(){
        m_nType = _default_protocol_download_rs;
    }
    char m_szFileName[MAXSIZE];
    long long m_fileId;
    long long m_pos;
    char m_szResult;
    long long m_filesize;
    char m_szFileUploadTime[MAXSIZE];
};
//下载读写协议包
struct STRU_downloadblock_RQ:public STRU_BASE{
    STRU_downloadblock_RQ(){
        m_nType = _default_protocol_downloadblock_rq;
    }
    long long m_fileId;
    char m_szFileContent[ONE_PAGE];
    long m_fileNum;
};

//删除
#define _login_deleteSuccess  0
#define _login_deleteErr      1
struct STRU_DELETEFILE_RQ:public STRU_BASE{
    STRU_DELETEFILE_RQ()
    {
        m_nType=_default_protocol_deletefile_rq;
    }
    long long m_userId;
    char m_fileName[MAXSIZE];
    long long m_FileSize;
    char m_szFileUploadTime[MAXSIZE];

};
struct STRU_DELETEFILE_RS:public STRU_BASE{
    STRU_DELETEFILE_RS()
    {
        m_nType=_default_protocol_deletefile_rs;
    }
    char m_szResult;
};
//搜索文件
#define _default_protocol_searchfile_rq    _default_protocol_base +21
#define _default_protocol_searchfile_rs    _default_protocol_base +22
#define _searchfile_Success  0
#define _searchfile_Err      1
struct STRU_SEARCHFILE_RQ:public STRU_BASE{
    STRU_SEARCHFILE_RQ(){
        m_nType = _default_protocol_searchfile_rq;
    }
    long long m_userId;
    char m_szFileName[MAXSIZE];

};
struct FileInfo{
    char m_szFileName[MAXSIZE];
    char m_szFileDateTime[MAXSIZE];
    long long m_fileSize;
};
struct STRU_SEARCHFILE_RS:public STRU_BASE{
    STRU_SEARCHFILE_RS(){
        m_nType = _default_protocol_searchfile_rs;
    }
    char m_szResult;
    FileInfo m_searchfileInfo[FILENUM];
   // char m_szFileName[MAXSIZE];
   // long long m_FileSize[MAXSIZE];
  //  char m_szFileUploadTime[MAXSIZE];
    char m_szCode[MAXSIZE];
    long long filelength;
};
//获取文件列表
struct STRU_GETFILELIST_RQ:public STRU_BASE{

    STRU_GETFILELIST_RQ(){
        m_nType = _default_protocol_getfilelist_rq;
    }
    long long m_userId;
};



struct STRU_GETFILELIST_RS:public STRU_BASE{

    STRU_GETFILELIST_RS(){
        m_nType = _default_protocol_getfilelist_rs;
    }
    FileInfo  m_aryInfo[FILENUM];
    long  m_FileNum;
};




//分享
struct STRU_SHARELINK_RQ:public STRU_BASE{
    STRU_SHARELINK_RQ(){
        m_nType = _default_protocol_sharelink_rq;
    }

    long long m_userId;
    char m_szFileName[MAXSIZE];
};
struct STRU_SHARELLINK_RS:public STRU_BASE{
    STRU_SHARELLINK_RS(){
        m_nType = _default_protocol_sharelink_rs;
    }
    char m_szLink[MAXSIZE];
    char m_szFileName[MAXSIZE];
    char m_szCode[MAXSIZE];
};
//提取
struct STRU_GETLINK_RQ:public STRU_BASE{
    STRU_GETLINK_RQ(){
        m_nType = _default_protocol_getlink_rq;
    }
    long long m_userId;
    char m_szCode[MAXSIZE];
};
struct STRU_GETLINK_RS:public STRU_BASE{
    STRU_GETLINK_RS(){
        m_nType = _default_protocol_getlink_rs;
    }
    long long m_userId;
    char m_szFileName[MAXSIZE];
    long long m_FileSize;
    char m_szFileUploadTime[MAXSIZE];
};

//申请账号
struct STRU_REGISTER_RQ:public STRU_BASE{

    STRU_REGISTER_RQ(){
        m_nType = _default_protocol_register_rq;
    }
    char m_szName[MAXSIZE];
    char m_szPassword[MAXSIZE];
    long long m_tel;
};
struct STRU_REGISTER_RS:public STRU_BASE{
    STRU_REGISTER_RS(){
        m_nType = _default_protocol_register_rs;
    }

    char m_szResult;
};
//登陆
//申请账号
struct STRU_LOGIN_RQ:public STRU_BASE{

    STRU_LOGIN_RQ(){
        m_nType = _default_protocol_login_rq;
    }
    char m_szName[MAXSIZE];
    char m_szPassword[MAXSIZE];

};
struct STRU_LOGIN_RS:public STRU_BASE{
    STRU_LOGIN_RS(){
        m_nType = _default_protocol_login_rs;
    }
    long long m_userId;
    char m_szResult;
};



struct STRU_UPLOADFILEINFO_RQ:public STRU_BASE{
    STRU_UPLOADFILEINFO_RQ(){
        m_nType = _default_protocol_uploadfileinfo_rq;
    }
    long long m_userId;
    char m_szFileName[MAXSIZE];
    long long m_filesize;
    char m_szFileMD5[MAXSIZE];
};
struct STRU_UPLOADFILEINFO_RS:public STRU_BASE{
    STRU_UPLOADFILEINFO_RS(){
        m_nType = _default_protocol_uploadfileinfo_rs;
    }
    char m_szFileName[MAXSIZE];
    long long m_fileId;
    long long m_pos;
    char m_szResult;
};
struct STRU_UPLOADFILEBLOCK_RQ:public STRU_BASE{
    STRU_UPLOADFILEBLOCK_RQ(){
        m_nType = _default_protocol_uploadfileblock_rq;
    }
    long long m_fileId;
    char m_szFileContent[ONE_PAGE];
    long m_fileNum;
};



#endif // PACKDEF_H
