#include "CMySql.h"


CMySql::CMySql(void)
{
    //QSqlDatabase::removeDatabase("qt_sql_default_connection");
    // m_pDataBase = new QSqlDatabase;
    m_pDataBase = QSqlDatabase::addDatabase("QODBC");
}


CMySql::~CMySql(void)
{

    //delete m_pDataBase;

}

void CMySql::DisConnect()
{
    m_pDataBase.close();
}

bool CMySql::ConnectMySql(const char *host,const char *user,const char *pass,const char *db)
{
    m_pDataBase.setHostName(host);
    m_pDataBase.setPort(3306);
    m_pDataBase.setUserName(user);
    m_pDataBase.setPassword(pass);
    m_pDataBase.setDatabaseName(db);
    if (!m_pDataBase.open()) {

     // qDebug() << QSqlDatabase::drivers();
      return false;
    }

    return true;
}

bool CMySql::SelectMySql(const char* szSql,int nColumn,list<string>& lstStr)
{
    //mysql_query() 函数用于向 MySQL 发送并执行 SQL 语句
      QSqlQuery result = m_pDataBase.exec(szSql);
     /*·mysql_store_result 对于成功检索了数据的每个查询(SELECT、SHOW、DESCRIBE、EXPLAIN、CHECK TABLE等)
     返回值:
     . CR_COMMANDS_OUT_OF_SYNC 　　以不恰当的顺序执行了命令。
 　　· CR_OUT_OF_MEMORY 　　内存溢出。
 　　· CR_SERVER_GONE_ERROR 　　MySQL服务器不可用。
 　　· CR_SERVER_LOST 　　在查询过程中，与服务器的连接丢失。
 　　· CR_UNKNOWN_ERROR 　　出现未知错误。*/


     QVariant value;
    //取出表格中第一行
    while (result.next())
    {
        //遍历列
         for(int i = 0;i < nColumn;i++)
         {
             value = result.value(i);
             if(value.isNull())
             {
                 value = (char*)"null";
             }
             lstStr.push_back(value.toString().toStdString());
         }


    }

    return true;
}

 bool  CMySql::UpdateMySql(const char* szSql)
 {
    if(!szSql)return false;

    QSqlQuery result = m_pDataBase.exec(szSql);
    QSqlError err = result.lastError();
    if(QSqlError::NoError !=err.type())
        return false;

    return true;
 }


