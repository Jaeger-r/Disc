#include "widget.h"
#include "ui_widget.h"
#include <QIcon>
#include "tou.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //ui->tableWidget->setColumnHidden(3,true);
    setWindowTitle("网盘");
    setWindowIcon(QIcon(WINDOWSICON));

    m_pKernel = new TCPKernel;
    m_plogin = new Login;

    m_plogin->setKernel(m_pKernel);
    m_plogin->show();
    if(m_pKernel->connect()){
        qDebug()<<"connect success";
    }else{
        QMessageBox::information(this,"err","connect error");
    }
    m_nFileNum = 0;
    row_ = 0;
    columncount_ = 0;

    int cpucount = QThread::idealThreadCount();
    QThreadPool *threadPool = QThreadPool::globalInstance();
    threadPool->setMaxThreadCount(cpucount/(1-0.8));

    //注册
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_register,m_plogin,&Login::slot_register,Qt::BlockingQueuedConnection);
    //登录
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_login,this,&Widget::slot_login,Qt::BlockingQueuedConnection);
    //获取文件列表
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_getfilelist,this,&Widget::slot_getfilelist,Qt::BlockingQueuedConnection);
    //上传文件
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_uploadfileinfo,this,&Widget::slot_uploadfileinfo,Qt::BlockingQueuedConnection);
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_uploadfileblock,this,&Widget::slot_uploadfileblock,Qt::BlockingQueuedConnection);
    //鼠标点击获取行信息
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &Widget::on_tableWidget_cellClicked);
    //删除文件
    connect((TCPKernel*)m_pKernel,&TCPKernel::signal_deletefileinfo,this,&Widget::slot_deletefile,Qt::BlockingQueuedConnection);
}

Widget::~Widget()
{
    delete ui;
    delete m_pKernel;
    delete m_plogin;
}
void Widget::slot_login(STRU_LOGIN_RS *pslr)
{
    const char *pszResult = "user no exist";
    if(pslr->m_szResult == _login_passworderr){
        pszResult = "Password Error";
    }
    if(pslr->m_szResult == _login_success){
        //关闭登录窗口
        m_plogin->hide();
        //显示主窗口
        this->show();
        m_userId = pslr->m_userId;
        //登陆成功
        //获取文件列表请求
        STRU_GETFILELIST_RQ sgr;
        sgr.m_userId = pslr->m_userId;
        m_pKernel->sendData((char*)&sgr,sizeof(sgr));
        return;
    }
    QMessageBox::information(this,"Login",pszResult);
}


void Widget::slot_getfilelist(STRU_GETFILELIST_RS *psgr)
{
    m_nFileNum = psgr->m_FileNum;
    QTableWidgetItem *pitem;
    for(int i = 0; i< psgr->m_FileNum;i++){
        //name
        pitem = new QTableWidgetItem(QIcon(FILEICON),psgr->m_aryInfo[i].m_szFileName);
        ui->tableWidget->setItem(i,0,pitem);
        //size
        QString strSize = QString::number(psgr->m_aryInfo[i].m_fileSize);
        pitem = new QTableWidgetItem(strSize);
        ui->tableWidget->setItem(i,1,pitem);
        //uploadtime
        pitem = new QTableWidgetItem(psgr->m_aryInfo[i].m_szFileDateTime);
        ui->tableWidget->setItem(i,2,pitem);
        //md5
        pitem = new QTableWidgetItem(psgr->m_aryInfo[i].m_szFileMD5);
        ui->tableWidget->setItem(i,3,pitem);
    }
}

void Widget::slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS * psur)
{
    QString strFileName = psur->m_szFileName;
    UploadFileInfo * pInfo = m_mapFileNameToFileInfo[strFileName];

    //处理文件相关内容
    if(psur->m_szResult == _fileinfo_isuploaded){
        QMessageBox::information(this,"UpLoadFile","The File has uploaded.");
        return;
    }else if(psur->m_szResult == _fileinfo_speedtransfer){
        //文件已经上传成功
        //1.将文件信息显示到控件上
        //2.提示文件上传成功
        QMessageBox::information(this,"UpLoadFile","UpLoadSuccess.");
        return;
    }else if(psur->m_szResult == _fileinfo_continue || psur->m_szResult == _fileinfo_normal){

        //将任务投递到线程池中
        // SendFileItask * task = new SendFileItask(pInfo,psur->m_pos,psur->m_fileId,this);
        // threadPool->start(task);
        FILE* pfile = fopen(pInfo->m_szFilePath,"rb");
        //如果是断点续传
        if(psur->m_pos){
            //将文件指针移动到指定位置
            fseek(pfile,psur->m_pos,SEEK_SET);
        }
        //正常发送文件内容
        STRU_UPLOADFILEBLOCK_RQ sur;
        strncpy(sur.m_szFileName, strFileName.toUtf8().data(), sizeof(sur.m_szFileName)-1);
        sur.m_fileId = psur->m_fileId;
        while(1){
            ssize_t nReadNum = fread(sur.m_szFileContent,sizeof(char),sizeof(sur.m_szFileContent),pfile);
            if(nReadNum){
                sur.m_fileNum = nReadNum;
                m_pKernel->sendData((char*)&sur,sizeof(sur));
            }else{
                break;
            }
        }
        fclose(pfile);
        //将映射map移除
        auto ite = m_mapFileNameToFileInfo.begin();
        while(ite != m_mapFileNameToFileInfo.end()){
            if((ite->second)->m_fileId == psur->m_fileId){
                m_mapFileNameToFileInfo.erase(ite);
                break;
            }
            ite++;
        }
    }
}

void Widget::slot_uploadfileblock(STRU_UPLOADFILEBLOCK_RS * uploadfileblock_rs)
{
    if (uploadfileblock_rs->m_szResult == _fileblock_success) {
        QString strFileName = QString::fromUtf8(uploadfileblock_rs->m_szFileName);
        UploadFileInfo * pInfo = m_mapFileNameToFileInfo[strFileName];
        //1.将文件信息显示在控件上
        QTableWidgetItem * p = new QTableWidgetItem(QIcon(FILEICON),strFileName);
        ui->tableWidget->setItem(m_nFileNum,0,p);

        QString strFileSize = QString::number(pInfo->m_fileSize);
        p = new QTableWidgetItem(strFileSize);
        ui->tableWidget->setItem(m_nFileNum,1,p);

        QDateTime datetime = QDateTime::currentDateTime();
        QTime time = QTime::currentTime();
        QString strDate = datetime.toString("dd.MM.yyyy");
        QString strTime = time.toString("HH,mm,ss");
        p = new QTableWidgetItem(strDate + " " + strTime);
        ui->tableWidget->setItem(m_nFileNum,2,p);

        p = new QTableWidgetItem(pInfo->m_szFileMD5);
        ui->tableWidget->setItem(m_nFileNum,3,p);
        ++m_nFileNum;
        //2.提示文件上传成功
        QMessageBox::information(this,"UpLoadFile","UpLoadSuccess.");
    }
    else if (uploadfileblock_rs->m_szResult == _fileblock_continue) {
        QMessageBox::information(this,"UpLoadFile","UpLoadContinue.");
    }
    else {
        QMessageBox::information(this,"UpLoadFile","UpLoadFail.");
    }
}

void Widget::slot_deletefile(STRU_DELETEFILE_RS * psds)
{
    if(psds->m_szResult == _delete_success){
        ui->tableWidget->removeRow(row_);
        QMessageBox::information(this,"Delete","The File has deleted.");
    }else{
        QMessageBox::information(this,"Delete","The File isn't exist.");
    }
}
string FileDigest(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return "";
    }

    MD5 md5;
    char buffer[1024];
    while (true) {
        qint64 length = file.read(buffer, 1024);
        if (length > 0) {
            md5.update(reinterpret_cast<const byte*>(buffer), static_cast<size_t>(length));
        } else {
            break;
        }
    }

    file.close();
    return md5.toString();
}
//MD5::update 函数使用的是 const byte* 类型的指针，而 QString 保存的是 Unicode 字符串
//因此在将 QString 转换为 const byte* 时可能会导致问题。
//你可以考虑使用 QString::toLocal8Bit() 将 QString 转换为本地字符集（默认为UTF-8）的 QByteArray
//然后使用 QByteArray::data() 获取 const char* 类型的指针。这样可以确保数据正确传递给 MD5::update 函数。

void Widget::on_pushButton_clicked()
{
    //获取上传文件信息（文件名，文件大小，文件MD5)
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),".","ALL files (*.*);;Images (*.png *.xpm *.jpg);;Text files (*.txt)");
    //QString fileName = filePath.section('/',-1);
    if (filePath.isEmpty()) {
        // 用户取消了选择
        QMessageBox::information(this,"FilePath","The FilePath is Empty.");
        return;
    }
    QString fileName = QFileInfo(filePath).fileName();
    long long filesize;
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly)){
        filesize = file.size();
        file.close();
    }
    string strMD5 = FileDigest(filePath);
    //上传文件信息请求
    STRU_UPLOADFILEINFO_RQ sur;
    sur.m_filesize = filesize;
    sur.m_userid = m_userId;
    memset(sur.m_szFileMD5, 0, sizeof(sur.m_szFileMD5));
    strncpy(sur.m_szFileMD5, strMD5.c_str(), sizeof(sur.m_szFileMD5) - 1);

    memset(sur.m_szFileName, 0, sizeof(sur.m_szFileName));
    strncpy(sur.m_szFileName, fileName.toStdString().c_str(), sizeof(sur.m_szFileName) - 1);


    m_pKernel->sendData((char*)&sur,sizeof(sur));

    //记录当前文件信息，文件名--（文件路径，文件大小，文件ID）
    UploadFileInfo *p = new UploadFileInfo;
    strcpy(p->m_szFilePath,filePath.toStdString().c_str());
    p->m_fileId = 0;
    p->m_fileSize = filesize;
    for(int i = 0;i < MAXSIZE;i++){
        p->m_szFileMD5[i] = sur.m_szFileMD5[i];
    }

    m_mapFileNameToFileInfo[fileName] = p;
}


void Widget::on_pushButton_2_clicked()
{
    //下载文件
    STRU_DOWNLOADFILEINFO_RQ sdq;
    strcpy(sdq.m_szFileMD5,m_MD5);
    m_pKernel->sendData((char*)&sdq,sizeof(sdq));
}


void Widget::on_pushButton_4_clicked()
{
    //删除文件
    STRU_DELETEFILE_RQ sdq;
    strcpy(sdq.m_szFileMD5,m_MD5);
    if(sdq.m_szFileMD5 != "fail"){
        m_pKernel->sendData((char*)&sdq,sizeof(sdq));
    }
}

void Widget::on_tableWidget_cellClicked(int row,int column)
{
    JaegerDebug();
    Q_UNUSED(column); // 忽略列信息
    int columnCount = ui->tableWidget->columnCount();
    columncount_=columnCount;
    row_=row;
    QTableWidgetItem *item = ui->tableWidget->item(row, 3);
    if (item) {
        strncpy(m_MD5,item->text().toStdString().c_str(),MAXSIZE-1);
    } else {
        strcpy(m_MD5,"fail");// 如果单元格为空
    }
}

void Widget::on_pushButton_9_clicked()
{
    //聊天
    m_chat->show();
}


void Widget::on_pushButton_5_clicked()
{
    //分享链接
    STRU_SHARELINK_RQ sharelink_rq;
}

