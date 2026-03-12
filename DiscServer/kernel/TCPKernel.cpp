#include "TCPKernel.h"
#include "tou.h"
#include <QDateTime>
TCPKernel *TCPKernel::m_pKernel = new TCPKernel;

TCPKernel::TCPKernel()
{
    m_pTCPNet = new TCPNet;
    m_pSQL = new CMySql;
    strcpy(m_szSystemPath,DISKPATH);
}

TCPKernel::~TCPKernel()
{
    delete m_pTCPNet;
    delete m_pSQL;
}

bool TCPKernel::open()
{

    if(!m_pTCPNet->initNetWork()){
        printf("Initialize NetWork Failed！\n");
        return false;
    }
    if(!m_pSQL->ConnectMySql("127.0.0.1","root","123456","disk2")){
        printf("Connect Mysql Failed！\n");
        return false;
    }
    outFile.open(CHATLOGPATH, ios::app); // 创建或打开txt文件，使用追加模式
    if (!outFile.is_open()) {
        cout << "Error opening file!" << endl;
    } else {
        cout << "File opened successfully!" << endl;
    }
    return true;
}

void TCPKernel::close()
{
    m_pTCPNet->unInitNetWork("over");
    m_pSQL->DisConnect();
    outFile.close();
}

void TCPKernel::dealData(SOCKET sock, char *szbuf)
{
    //todo
    switch(*szbuf){
    case _default_protocol_register_request:
        Register_Request(sock,szbuf);
        break;
    case _default_protocol_login_request:
        Login_Request(sock,szbuf);
        break;
    case _default_protocol_getfilelist_request:
        GetFileList_Request(sock,szbuf);
        break;
    case _default_protocol_uploadfileinfo_request:
        UploadFileInfo_Request(sock,szbuf);
        break;
    case _default_protocol_uoloadfileblock_request:
        UploadFileBlock_Request(sock,szbuf);
        break;
    case _default_protocol_downloadfileinfo_request:
        DownloadFileInfo_Request(sock,szbuf);
        break;
    case _default_protocol_downloadfileblock_request:
        DownFileBlock_Request(sock,szbuf);
        break;
    case _default_protocol_chat_request:
        SendMessage_Request(sock,szbuf);
        break;
    }
}


// #define _login_usernoexist 0
// #define _login_passworderr 1
// #define _login_success 2
void TCPKernel::Login_Request(SOCKET sock, char *szbuf)
{
    STRU_LOGIN_RQ *login_rq = (STRU_LOGIN_RQ*) szbuf;
    STRU_LOGIN_RS login_rs;

    char szsql[SQLLEN] = {0};
    list<string> liststr;

    //初始化为用户不存在
    login_rs.m_szResult = _login_usernoexist;
    sprintf(szsql,"select u_id,u_password from user where u_name = '%s';",login_rq->m_szName);
    m_pSQL->SelectMySql(szsql,2,liststr);

    //校验是否存在用户
    if(liststr.size() > 0){
        login_rs.m_szResult = _login_passworderr;
        //校验密码是否成功
        string strUserId = liststr.front();
        liststr.pop_front();
        string strpassword = liststr.front();
        liststr.pop_front();
        if(strcmp(login_rq->m_szPassWord,strpassword.c_str())==0){
            //密码成功
            login_rs.m_szResult = _login_success;
            login_rs.m_userId = atoll(strUserId.c_str());
            cout<<"success"<<endl;
        }else{
            cout<<"password is not correct"<<endl;
        }
    }else{
        cout<<"user is not exist"<<endl;
    }
    //发送回复
    m_pTCPNet->sendData(sock,(char*)&login_rs,sizeof(login_rs));
}
//获取文件列表
void TCPKernel::GetFileList_Request(SOCKET sock, char *szbuf)
{
    STRU_GETFILELIST_RQ *getfilelist_rq = (STRU_GETFILELIST_RQ*)szbuf;
    char szsql[SQLLEN] = {0};
    list<string> liststr;
    STRU_GETFILELIST_RS getfilelist_rs;
    sprintf(szsql,"select f_name,f_size,f_uploadtime,f_md5 from myview where u_id = %lld;",getfilelist_rq->m_userId);
    m_pSQL->SelectMySql(szsql,4,liststr);
    int i = 0;
    while(liststr.size() > 0){

        string strFileName = liststr.front();
        liststr.pop_front();
        string strFileSize = liststr.front();
        liststr.pop_front();
        string strFileUploadtime = liststr.front();
        liststr.pop_front();
        string strFilemd5 = liststr.front();
        liststr.pop_front();

        strcpy(getfilelist_rs.m_aryInfo[i].m_szFileName,strFileName.c_str());
        strcpy(getfilelist_rs.m_aryInfo[i].m_szFileDateTime,strFileUploadtime.c_str());
        getfilelist_rs.m_aryInfo[i].m_fileSize = atoll(strFileSize.c_str());
        strcpy(getfilelist_rs.m_aryInfo[i].m_szFileMD5,strFilemd5.c_str());

        i++;
        if(i == FILENUM || liststr.size() == 0){
            getfilelist_rs.m_FileNum = i;
            m_pTCPNet->sendData(sock,(const char*)&getfilelist_rs,sizeof(getfilelist_rs));
            ZeroMemory(getfilelist_rs.m_aryInfo,sizeof(getfilelist_rs.m_aryInfo));
            i = 0;
        }
    }
}
//上传文件
void TCPKernel::UploadFileInfo_Request(SOCKET sock, char *szbuf)
{
    JaegerDebug();
    STRU_UPLOADFILEINFO_RQ*uploadfileinfo_rq = (STRU_UPLOADFILEINFO_RQ*)szbuf;
    char szsql[SQLLEN] = {0};
    list<string> lststr;
    STRU_UPLOADFILEINFO_RS uploadfileinfo_rs;

    strcpy(uploadfileinfo_rs.m_szFileName,uploadfileinfo_rq->m_szFileName);
    uploadfileinfo_rs.m_pos = 0;

    sprintf(szsql,"select u_id,f_id,f_count,f_pos from myview where f_md5 = '%s'",uploadfileinfo_rq->m_szFileMD5);
    m_pSQL->SelectMySql(szsql,4,lststr);
    //1.判断数据库是否存在热门文件
    if(lststr.size()>0){
        string strUserId = lststr.front();lststr.pop_front();
        string strFileId = lststr.front();lststr.pop_front();
        string strFileCount = lststr.front();lststr.pop_front();
        string strFilePos = lststr.front();lststr.pop_front();

        long long userId = atoll(strUserId.c_str());
        long long fileId = atoll(strFileId.c_str());
        long long filepos = atoll(strFilePos.c_str());

        uploadfileinfo_rs.m_fileId = fileId;

        if(uploadfileinfo_rq->m_userid == userId){
            //1.1判断文件是不是自己传过的 1.没有传完---断点续传 2.传完了——重复上传
            if(filepos >= uploadfileinfo_rq->m_filesize) {
                uploadfileinfo_rs.m_szResult = _fileinfo_isuploaded;
            }else{
                uploadfileinfo_rs.m_szResult = _fileinfo_continue;
                uploadfileinfo_rs.m_pos = filepos;
            }
            //没有传完---断点续传
            // uploadFileInfo *pInfo = m_mapFileToFileInfo[fileId];
            // if(pInfo){
            //     uploadfileinfo_rs.m_szResult = _fileinfo_continue;
            //     uploadfileinfo_rs.m_pos = pInfo->m_pos;
            // }
        }else{
            //1.2如果文件是别人传过的，存在则秒传成功———文件引用计数+1
            uploadfileinfo_rs.m_szResult =_fileinfo_speedtransfer;
            sprintf(szsql,"update file set f_count = f_count +1 where f_id = %lld;",fileId);
            m_pSQL->UpdateMySql(szsql);
            //1.3将文件信息与用户信息 映射到user——file
            sprintf(szsql,"insert into user_file(u_id,f_id) values(%lld,%lld);",userId,fileId);
            m_pSQL->UpdateMySql(szsql);
        }
    }else{
        //2.数据库不存在文件--正常传
        uploadfileinfo_rs.m_szResult = _fileinfo_normal;
        char szFilePath[MAX_PATH] = {0};
        sprintf(szFilePath,"%s%lld/%s",m_szSystemPath,uploadfileinfo_rq->m_userid,uploadfileinfo_rq->m_szFileName);
        sprintf(szsql,"insert into file(f_name,f_size,f_uploadtime,f_path,f_md5,f_pos) values('%s',%lld,NOW(),'%s','%s',%d)",
                uploadfileinfo_rq->m_szFileName,uploadfileinfo_rq->m_filesize,szFilePath,uploadfileinfo_rq->m_szFileMD5,0);
        //2.1将文件信息加入数据库中
        m_pSQL->UpdateMySql(szsql);
        //2.2将文件与用户信息映射到user——file
        sprintf(szsql,"select f_id from file where f_md5 = '%s'",uploadfileinfo_rq->m_szFileMD5);
        m_pSQL->SelectMySql(szsql,1,lststr);
        if(lststr.size() >0){
            string strFileId = lststr.front();
            lststr.pop_front();
            uploadfileinfo_rs.m_fileId = atoll(strFileId.c_str());
            sprintf(szsql,"insert into user_file(u_id,f_id) values(%lld,%lld);",
                    uploadfileinfo_rq->m_userid,atoll(strFileId.c_str()));
            m_pSQL->UpdateMySql(szsql);
        }
        //2.3创建新文件
        FILE *pfile = fopen(szFilePath,"wb");
        //记录文件信息--fileid,--(文件指针，文件大小，文件位置，userid)
        uploadFileInfo *p = new uploadFileInfo;
        p->m_pFile = pfile;
        p->m_fileSize = uploadfileinfo_rq->m_filesize;
        p->m_lastDBPos = 0;
        p->m_pos = 0;
        p->m_userId = uploadfileinfo_rq->m_userid;
        for(int i = 0;i < MAXSIZE;i++){
            p->m_szFileMD5[i] = uploadfileinfo_rq->m_szFileMD5[i];
        }
        m_mapFileToFileInfo[uploadfileinfo_rs.m_fileId] = p;
    }
    m_pTCPNet->sendData(sock,(char*)&uploadfileinfo_rs,sizeof(uploadfileinfo_rs));
}

void TCPKernel::UploadFileBlock_Request(SOCKET sock, char *szbuf)
{
    JaegerDebug();
    //将文件内容写到对应的文件中
    STRU_UPLOADFILEBLOCK_RQ *uploadfileblock_rq = (STRU_UPLOADFILEBLOCK_RQ*) szbuf;
    if(!uploadfileblock_rq)return;

    auto ite = m_mapFileToFileInfo.find(uploadfileblock_rq->m_fileId);
    if(ite == m_mapFileToFileInfo.end()) {
        //未找到文件
        return;
    }
    uploadFileInfo *p = ite->second;
    if(!p||!p->m_pFile)return;//文件指针无效

    size_t nWriteNum = fwrite(uploadfileblock_rq->m_szFileContent,
                              sizeof(char),
                              uploadfileblock_rq->m_fileNum,
                              p->m_pFile);
    if(nWriteNum != uploadfileblock_rq->m_fileNum){
        qDebug()<< "文件写入失败或未写完整块，文件ID："<<uploadfileblock_rq->m_fileId;
        return;
    }

    p->m_pos += nWriteNum;
    const long long UPDATE_STEP = 1024 * 1024; // 1MB更新一次

    if(p->m_pos - p->m_lastDBPos >= UPDATE_STEP
        || p->m_pos >= p->m_fileSize)
    {
        char szsql[SQLLEN] = {0};

        sprintf(szsql,
                "update file f_pos = %lld where f_id = %lld;",
                p->m_pos,
                uploadfileblock_rq->m_fileId);

        m_pSQL->UpdateMySql(szsql);

        p->m_lastDBPos = p->m_pos;
    }

    if(p->m_pos >= p->m_fileSize){
        //文件上传完毕
        if(p->m_pFile) {
            fclose(p->m_pFile);
            p->m_pFile = nullptr;
        }

        //释放内存删除映射表
        delete p;
        m_mapFileToFileInfo.erase(ite);
        //发送结果信息
        STRU_UPLOADFILEBLOCK_RS uploadfileblock_rs;
        strcpy(uploadfileblock_rs.m_szFileName,uploadfileblock_rq->m_szFileName);
        uploadfileblock_rs.m_szResult = _fileblock_success;
        m_pTCPNet->sendData(sock,(char*)&uploadfileblock_rs,sizeof(uploadfileblock_rs));
        return;
    }
    //发送未完成结果
    // STRU_UPLOADFILEBLOCK_RS uploadfileblock_rs;
    // uploadfileblock_rs.m_fileId = uploadfileblock_rq->m_fileId;
    // strcpy(uploadfileblock_rs.m_szFileName,uploadfileblock_rq->m_szFileName);
    // uploadfileblock_rs.m_szResult = _fileblock_continue;
    // uploadfileblock_rs.m_pos = p->m_pos;
    // m_pTCPNet->sendData(sock,(char*)&uploadfileblock_rs,sizeof(uploadfileblock_rs));
    // return;
}

void TCPKernel::DownloadFileInfo_Request(SOCKET sock, char *szbuf)
{
    //下载回复
    STRU_DOWNLOADFILEINFO_RQ * downloadfileinfo_rq = (STRU_DOWNLOADFILEINFO_RQ*) szbuf;
    char szsql[SQLLEN] = {0};
    list<string> lststr;
    STRU_DOWNLOADFILEINFO_RS downloadfileinfo_rs;
    //strcpy(downloadfileinfo_rs.m_szFileMD5,downloadfileinfo_rq->m_szFileName);
    downloadfileinfo_rs.m_pos = 0;
    sprintf(szsql,"select f_id,f_name,f_count from myview where f_md5 = '%s'",downloadfileinfo_rq->m_szFileMD5);
    m_pSQL->SelectMySql(szsql,3,lststr);
    if (lststr.size() > 0) {
        string strFileId = lststr.front();
        lststr.pop_front();
        string strFileName = lststr.front();
        lststr.pop_front();
        string strFileCount = lststr.front();
        lststr.pop_front();
        long long fileId = atoll(strFileId.c_str());
        downloadfileinfo_rs.m_fileId = fileId;
    }
}

void TCPKernel::DownFileBlock_Request(SOCKET sock, char *szbuf)
{

}

void TCPKernel::DeleteFile_Request(SOCKET sock, char *szbuf)
{
    //删除文件
    STRU_DELETEFILE_RQ *psdq = (STRU_DELETEFILE_RQ*)szbuf;
    char szsql[SQLLEN] = {0};
    list<string> lststr;
    STRU_DELETEFILE_RS sds;
    sds.m_szResult = _delete_noexit;
    sprintf(szsql,"select f_id from file where f_md5 = '%s'",psdq->m_szFileMD5);
    m_pSQL->SelectMySql(szsql,1,lststr);
    if(lststr.size()>0){
        string strFileId = lststr.front();
        lststr.pop_front();
        long long strfileid = atoll(strFileId.c_str());
        sprintf(szsql,"delete from file where f_md5 = '%s'",psdq->m_szFileMD5);
        m_pSQL->UpdateMySql(szsql);
        sprintf(szsql,"delete from user_file where f_id = '%s'",strfileid);
        m_pSQL->UpdateMySql(szsql);
        sprintf(szsql,"delete from myview where f_md5 = '%s'",psdq->m_szFileMD5);
        m_pSQL->UpdateMySql(szsql);
        sds.m_szResult = _delete_success;
        m_pTCPNet->sendData(sock,(char*)&sds,sizeof(sds));
    }else{
        m_pTCPNet->sendData(sock,(char*)&sds,sizeof(sds));
    }
}

void TCPKernel::SendMessage_Request(SOCKET sock, char *szbuf)
{
    STRU_CHAT_RQ* chat_request = (STRU_CHAT_RQ*)szbuf;
    STRU_CHAT_RS chat_send;
    // 获取当前时间
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char timeStr[128];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
    // 将接收到的消息广播给所有客户端
    strcpy(chat_send.szbuf,chat_request->szbuf);
    for (const auto& entry : m_pTCPNet->getThreadIdToSocketMap()){
        SOCKET client = entry.second;
        if(sock!=client){
            m_pTCPNet->sendData(client, (char*)&chat_send, sizeof(chat_send));
        }
    }
    // 将聊天内容记录到log文件中
    std::cout<<"System Record:"<<"User "<<chat_send.m_userName<<":"<<chat_send.szbuf<<std::endl;
    string message = "[" + string(timeStr) + "] " + string("User")+string(chat_send.m_userName)+string(":")+string(chat_send.szbuf) + "\n";
    outFile << message;
    outFile.flush(); // 立即将数据写入文件
}
