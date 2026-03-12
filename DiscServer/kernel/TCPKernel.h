#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include "IKernel.h"
#include"../server/tcpnet.h"
#include "CMySql.h"
#include "Packdef.h"
#include "tou.h"
#include <iostream>
#include <map>
#include <fstream>  // 添加头文件以使用文件操作
#include <string>   // 添加头文件以使用string类型
#include <ctime>    // 添加头文件以获取时间信息
#include <algorithm>  // 包含头文件以使用 std::find
//记录文件信息--fileid,--(文件指针，文件大小，文件位置，userid)
struct uploadFileInfo{
    FILE * m_pFile;
    long long m_fileSize;
    long long m_pos;
    long long m_lastDBPos;
    long long m_userId;
    char m_szFileMD5[MAXSIZE];
};

using namespace std;

class TCPKernel : public IKernel
{
private:
    TCPKernel() ;
    ~TCPKernel();
public:
    virtual bool open();
    virtual void close();
    virtual void dealData(SOCKET sock,char*szbuf);
public:
    void Register_Request(SOCKET sock,char* szbuf)
    {
        STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ*)szbuf;
        char szsql[SQLLEN] = {0};
        char szpath[MAX_PATH] = {0};
        STRU_REGISTER_RS srr;
        srr.m_szResult = _register_err;
        list<string>lststr;
        //将数据插入到数据库中
        sprintf(szsql,"insert into user(u_name,u_password,u_tel) values ('%s','%s',%lld)",
                psrr->m_szName,psrr->m_szPassWord,psrr->m_tel);
        if(m_pSQL->UpdateMySql(szsql)){
            //更新成功
            srr.m_szResult = _register_success;
            //获取用户id
            sprintf(szsql,"select u_id, u_name from user where u_tel = %lld",psrr->m_tel);
            m_pSQL->SelectMySql(szsql,2,lststr);
            if(lststr.size()>0){
                string strUserId = lststr.front();
                lststr.pop_front();
                string strUserName = lststr.front();
                lststr.pop_front();
                sprintf(szpath,"%s%s",m_szSystemPath,strUserId.c_str());
                //为当前用户创建文件
                CreateDirectoryA(szpath,0);
            }
        }
        //发送回复 -- 序列化  -- 反序列化
        m_pTCPNet->sendData(sock,(char*)&srr,sizeof(srr));
    }
    void Login_Request(SOCKET sock,char* szbuf);
    void GetFileList_Request(SOCKET sock,char* szbuf);
    void UploadFileInfo_Request(SOCKET sock,char* szbuf);
    void UploadFileBlock_Request(SOCKET sock,char* szbuf);
    void DownloadFileInfo_Request(SOCKET sock,char* szbuf);
    void DownFileBlock_Request(SOCKET sock,char* szbuf);
    void DeleteFile_Request(SOCKET sock,char* szbuf);
    void SendMessage_Request(SOCKET sock,char* szbuf);
public:
    //单例模式--不支持线程安全
    //饿汉模式，支持线程安全 高效
    static IKernel * getKernel(){
        return m_pKernel;
    }
private:
    INet *m_pTCPNet;
    CMySql *m_pSQL;
    static TCPKernel *m_pKernel;
    char m_szSystemPath[MAX_PATH];
    std::map<long long,uploadFileInfo *> m_mapFileToFileInfo;
    ofstream outFile;
};

#endif // TCPKERNEL_H
