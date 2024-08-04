#pragma once
#include <mysql.h>

//#include <WinSock2.h>
//#pragma comment(lib,"../mysql/libmysql.lib")
//
#include <list>
#include <string>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:
    bool  ConnectMySql(const char *host,const char *user,const char *pass,const char *db);
    void  DisConnect();
    bool  SelectMySql(const char* szSql,int nColumn,list<string>& lstStr);
   
    //���£�ɾ�������롢�޸�
    bool  UpdateMySql(const char* szSql);
 
private:
    MYSQL *sock;   
	MYSQL_RES *results;   
	MYSQL_ROW record; 
   
};

