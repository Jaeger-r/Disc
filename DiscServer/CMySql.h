#ifndef CMYSQL_H
#define CMYSQL_H
#include <QtSql>
#include <QSqlQuery>
#include <list>
#include <string>
#include <QVariant>
#include <QSqlError>
#include <C:\Program Files\MySQL\MySQL Server 8.0\include\mysql.h>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:               //ip,                   用户名，    密码，            数据库 ，3306
    bool  ConnectMySql(const char *host,const char *user,const char *pass,const char *db);
    void  DisConnect();
    bool  SelectMySql(const char* szSql,int nColumn,list<string>& lstStr);

    //更新：删除、插入、修改
    bool  UpdateMySql(const char* szSql);

private:
   QSqlDatabase m_pDataBase;

};
#endif // CMYSQL_H
