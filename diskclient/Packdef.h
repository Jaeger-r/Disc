#ifndef PACKDEF_H
#define PACKDEF_H
#include <QString>
#define _default_protocol_base  10

//注册
#define _default_protocol_register_request                _default_protocol_base +1
#define _default_protocol_register_send                   _default_protocol_base +2
//登录
#define _default_protocol_login_request                   _default_protocol_base +3
#define _default_protocol_login_send                      _default_protocol_base +4
//获取文件列表
#define _default_protocol_getfilelist_request             _default_protocol_base +5
#define _default_protocol_getfilelist_send                _default_protocol_base +6
//上传文件  正常传文件，断点续传、秒传
#define _default_protocol_uploadfileinfo_request          _default_protocol_base +7
#define _default_protocol_uploadfileinfo_send             _default_protocol_base +8
#define _default_protocol_uoloadfileblock_request         _default_protocol_base +9
#define _default_protocol_uoloadfileblock_send            _default_protocol_base +10
//下载文件  正常传文件，断点续传、秒传
#define _default_protocol_downloadfileinfo_request        _default_protocol_base +11
#define _default_protocol_downloadfileinfo_send           _default_protocol_base +12
#define _default_protocol_downloadfileblock_request       _default_protocol_base +13
#define _default_protocol_downloadfileblock_send          _default_protocol_base +14
//搜索文件
#define _default_protocol_searchfile_request              _default_protocol_base +15
#define _default_protocol_searchfile_send                 _default_protocol_base +16
//删除文件
#define _default_protocol_deletefile_request              _default_protocol_base +17
#define _default_protocol_deletefile_send                 _default_protocol_base +18
//分享
#define _default_protocol_sharelink_request               _default_protocol_base + 19
#define _default_protocol_sharelink_send                  _default_protocol_base + 20
//提取
#define _default_protocol_getlink_request                 _default_protocol_base + 21
#define _default_protocol_getlink_send                    _default_protocol_base + 22
//聊天
#define _default_protocol_chat_request                    _default_protocol_base + 23
#define _default_protocol_chat_send                       _default_protocol_base + 24



//协议包

#define MAXSIZE 45
#define NAMESIZE 100
#define FILENUM 15
#define SQLLEN  300
#define ONE_PAGE 4096
#define MAXSENDMESSSAGE 1024
#define _register_err 0
#define _register_success 1

#define _login_usernoexist 0
#define _login_passworderr 1
#define _login_success 2

#define _fileinfo_isuploaded 0       //已经上传
#define _fileinfo_continue   1       //断点续传
#define _fileinfo_speedtransfer 2    //秒传
#define _fileinfo_normal     3       //正常传

#define _fileblock_success 1         //上传成功
#define _fileblock_continue 2        //未传完整继续上传
#define _fileblock_fail 3            //上传失败

#define _delete_success 0
#define _delete_fail 1
#define _delete_noexit 2


struct STRU_BASE{
    char m_nType;//包类型
};


/*
    申请账号
*/
struct STRU_REGISTER_RQ:public STRU_BASE{
    STRU_REGISTER_RQ(){
        m_nType = _default_protocol_register_request;
    }
    char m_szName[MAXSIZE];
    char m_szPassWord[MAXSIZE];
    long long m_tel;
};
struct STRU_REGISTER_RS:public STRU_BASE{
    STRU_REGISTER_RS(){
        m_nType = _default_protocol_register_send;
    }
    char m_szResult;
};


/*
    登录
*/
struct STRU_LOGIN_RQ:public STRU_BASE{
    STRU_LOGIN_RQ(){
        m_nType = _default_protocol_login_request;
    }
    char m_szName[MAXSIZE];
    char m_szPassWord[MAXSIZE];
};
struct STRU_LOGIN_RS:public STRU_BASE{
    STRU_LOGIN_RS(){
        m_nType = _default_protocol_login_send;
    }
    long long m_userId;
    char m_szResult;
};


/*
    获取文件列表
*/
struct STRU_GETFILELIST_RQ:public STRU_BASE{
    STRU_GETFILELIST_RQ(){
        m_nType = _default_protocol_getfilelist_request;
    }
    long long m_userId;
};
struct FileInfo{
    char m_szFileName[MAXSIZE];
    char m_szFileDateTime[MAXSIZE];
    long long m_fileSize;
    char m_szFileMD5[MAXSIZE];
};
struct STRU_GETFILELIST_RS:public STRU_BASE{
    STRU_GETFILELIST_RS(){
        m_nType = _default_protocol_getfilelist_send;
    }
    FileInfo m_aryInfo[FILENUM];
    long m_FileNum;
};


/*
    上传文件前置
*/
struct STRU_UPLOADFILEINFO_RQ : public STRU_BASE{
    STRU_UPLOADFILEINFO_RQ(){
        m_nType = _default_protocol_uploadfileinfo_request;
    }
    long long m_userid;
    char m_szFileName[MAXSIZE];
    long long m_filesize;
    char m_szFileMD5[MAXSIZE];
};
struct STRU_UPLOADFILEINFO_RS : public STRU_BASE{
    STRU_UPLOADFILEINFO_RS(){
        m_nType = _default_protocol_uploadfileinfo_send;
    }
    char m_szFileName[MAXSIZE];
    long long m_fileId;
    long long m_pos;
    char m_szResult;
};


/*
    上传文件内容
*/
struct STRU_UPLOADFILEBLOCK_RQ : public STRU_BASE{
    STRU_UPLOADFILEBLOCK_RQ(){
        m_nType = _default_protocol_uoloadfileblock_request;
    }
    char m_szFileName[MAXSIZE];
    long long m_fileId;
    char m_szFileContent[ONE_PAGE];
    long m_fileNum;
};
struct STRU_UPLOADFILEBLOCK_RS : public STRU_BASE{
    STRU_UPLOADFILEBLOCK_RS(){
        m_nType = _default_protocol_uoloadfileblock_send;
    }
    char m_szFileName[MAXSIZE];
    char m_szResult;
};


/*
    下载文件前置
*/
struct STRU_DOWNLOADFILEINFO_RQ : public STRU_BASE{
    STRU_DOWNLOADFILEINFO_RQ(){
        m_nType = _default_protocol_downloadfileinfo_request;
    }
    long long m_userid;
    char m_szFileName[MAXSIZE];
    long long m_filesize;
    char m_szFileMD5[MAXSIZE];
};
struct STRU_DOWNLOADFILEINFO_RS : public STRU_BASE{
    STRU_DOWNLOADFILEINFO_RS(){
        m_nType = _default_protocol_uploadfileinfo_send;
    }
    char m_szFileName[MAXSIZE];
    long long m_fileId;
    long long m_pos;
    char m_szResult;
};


/*
    下载文件内容
*/
struct STRU_DOWNLOADFILEBLOCK_RQ : public STRU_BASE{
    STRU_DOWNLOADFILEBLOCK_RQ(){
        m_nType = _default_protocol_uoloadfileblock_request;
    }
    long long m_fileId;
    char m_szFileContent[ONE_PAGE];
    long m_fileNum;
};


/*
    删除文件
*/
struct STRU_DELETEFILE_RQ:public STRU_BASE{
    STRU_DELETEFILE_RQ(){
        m_nType = _default_protocol_deletefile_request;
    }
    long long m_userId;
    char m_szFileMD5[MAXSIZE];
};
struct STRU_DELETEFILE_RS:public STRU_BASE{
    STRU_DELETEFILE_RS(){
        m_nType = _default_protocol_deletefile_send;
    }
    long long m_userId;
    char m_szResult;
    char m_szFileMD5[MAXSIZE];
};


/*
    聊天
*/
struct STRU_CHAT_RQ:public STRU_BASE{
    STRU_CHAT_RQ(){
        m_nType = _default_protocol_chat_request;
    }
    char m_userName[MAXSIZE];
    char szbuf[MAXSENDMESSSAGE];
};
struct STRU_CHAT_RS:public STRU_BASE{
    STRU_CHAT_RS(){
        m_nType = _default_protocol_chat_send;
    }
    int user_id;
    char m_userName[MAXSIZE];
    char szbuf[MAXSENDMESSSAGE];
};


/*
    分享
*/
struct STRU_SHARELINK_RQ:public STRU_BASE{
    STRU_SHARELINK_RQ(){
        m_nType = _default_protocol_sharelink_request;
    }
    long long m_userId;
    long long m_fileId;
};
struct STRU_SHARELINK_RS:public STRU_BASE{
    STRU_SHARELINK_RS(){
        m_nType = _default_protocol_sharelink_send;
    }
    char m_sharepwd[MAXSIZE];
    char m_szCode[MAXSIZE];
};


//提取
struct STRU_GETLINK_RQ:public STRU_BASE{
    STRU_GETLINK_RQ(){
        m_nType = _default_protocol_getlink_request;
    }
    long long m_userId;
    char m_szCode[MAXSIZE];
};
struct STRU_GETLINK_RS:public STRU_BASE{
    STRU_GETLINK_RS(){
        m_nType = _default_protocol_getlink_send;
    }
    long long m_userId;
    char m_szFileName[MAXSIZE];
    long long m_FileSize;
    char m_szFileUploadTime[MAXSIZE];
};
/*
 * 用户表user（u_id,u_name,password,u_tel)
 * 文件信息表 file(f_id,f——name,f_size,f_upoloadtime,f_path,f_count,f_md5)
 * user_file(num,u_id,f_id)

*/
#endif // PACKDEF_H












