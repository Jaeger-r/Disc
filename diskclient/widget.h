#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "./kernel/tcpkernel.h"
#include "chat.h"
#include "login.h"
#include <QDebug>
#include <QMessageBox>
#include <qfiledialog.h>
#include "md5.h"
#include <map>
#include <QDateTime>
#include <QTime>
#include <QRunnable>
#include <QThreadPool>
#include <windows.h>
#include <QSysInfo>
#include <QThread>
//文件名--（文件路径，文件大小，文件ID）
struct UploadFileInfo{
    char m_szFilePath[MAX_PATH];
    long long m_fileSize;
    long long m_fileId;
    char m_szFileMD5[MAXSIZE];
};

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void slot_login(STRU_LOGIN_RS *);
    void slot_getfilelist(STRU_GETFILELIST_RS*);
    void slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS*);
    void slot_uploadfileblock(STRU_UPLOADFILEBLOCK_RS*);
    void slot_deletefile(STRU_DELETEFILE_RS*);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_9_clicked();
    void on_tableWidget_cellClicked(int row,int column);
    void on_pushButton_5_clicked();

private:
    char m_MD5[MAXSIZE];
    QThreadPool *threadPool;
    Ui::Widget *ui;
    Login * m_plogin;
    long long m_userId;
    int m_nFileNum;
    int row_;
    int columncount_;
public:
    std::map<QString,UploadFileInfo*> m_mapFileNameToFileInfo;
    IKernel * m_pKernel;
    Chat * m_chat;
};

class SendFileItask:public QRunnable{
public:
    SendFileItask(UploadFileInfo*pInfo,long long pos,long long fileId,Widget *pWidget){
        m_pInfo = pInfo;
        m_pos = pos;
        m_fileId = fileId;
        m_pWidget = pWidget;
    }
    ~SendFileItask(){

    }
public:
    void run()override{
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
            if(nReadNum){
                sur.m_fileNum = nReadNum;
                m_pWidget->m_pKernel->sendData((char*)&sur,sizeof(sur));
            }else{
                break;
            }
        }
        fclose(pfile);
        //将映射map移除
        auto ite = m_pWidget->m_mapFileNameToFileInfo.begin();
        while(ite != m_pWidget->m_mapFileNameToFileInfo.end()){
            if((ite->second)->m_fileId == m_fileId){
                m_pWidget->m_mapFileNameToFileInfo.erase(ite);
                break;
            }
            ite++;
        }
    };
private:
    UploadFileInfo* m_pInfo;
    long long m_pos;
    long long m_fileId;
    Widget *m_pWidget;

};
// class DownLoadFileItask:public QRunnable{
// public:
//     DownLoadFileItask(UploadFileInfo*pInfo,long long pos,long long fileId,Widget *pWidget){
//         m_pInfo = pInfo;
//         m_pos = pos;
//         m_fileId = fileId;
//         m_pWidget = pWidget;
//     }
//     ~DownLoadFileItask(){

//     }
// public:
//     void run()override{
//         FILE* pfile = fopen(m_pInfo->m_szFilePath,"rb");
//         //如果是断点续传
//         if(m_pos){
//             //将文件指针移动到指定位置
//             fseek(pfile,m_pos,SEEK_SET);
//         }
//         //正常发送文件内容
//         STRU_UPLOADFILEBLOCK_RQ sur;
//         sur.m_fileId = m_fileId;
//         while(1){
//             ssize_t nReadNum = fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),pfile);
//             if(nReadNum){
//                 sur.m_fileNum = nReadNum;
//                 m_pWidget->m_pKernel->sendData((char*)&sur,sizeof(sur));
//             }else{
//                 break;
//             }
//         }
//         fclose(pfile);
//         //将映射map移除
//         auto ite = m_pWidget->m_mapFileNameToFileInfo.begin();
//         while(ite != m_pWidget->m_mapFileNameToFileInfo.end()){
//             if((ite->second)->m_fileId == m_fileId){
//                 m_pWidget->m_mapFileNameToFileInfo.erase(ite);
//                 break;
//             }
//             ite++;
//         }
//     };
// private:
//     UploadFileInfo* m_pInfo;
//     long long m_pos;
//     long long m_fileId;
//     Widget *m_pWidget;

// };
#endif // WIDGET_H
