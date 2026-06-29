#include "widget.h"

#include <QAbstractItemView>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPoint>
#include <QBrush>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

#include "md5.h"

namespace {
QString currentTimestamp()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

}

Widget::Widget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("diskMainWindow"));
    buildUi();
    applyStyle();

    setWindowTitle(QStringLiteral("Jaeger Disk"));
    setWindowIcon(QIcon(QStringLiteral(":/app/DiskClientIcon.png")));
    resize(1180, 760);

    m_uploadPumpTimer.setInterval(10);
    connect(&m_uploadPumpTimer, &QTimer::timeout, this, &Widget::processUploadPump);
    m_transferRateTimer.setInterval(1000);
    connect(&m_transferRateTimer, &QTimer::timeout, this, &Widget::updateTransferRates);
    m_transferRateTimer.start();

    m_kernel = new TCPKernel(this);
    m_login = new Login;
    m_login->setKernel(m_kernel);
    m_chat = new Chat;
    m_chat->setKernel(m_kernel);
    m_transferPanel = new TransferPanel;

    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_register, m_login, &Login::slot_register);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_login, this, &Widget::slot_login);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_getfilelist, this, &Widget::slot_getfilelist);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_uploadfileinfo, this, &Widget::slot_uploadfileinfo);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_uploadfileblock, this, &Widget::slot_uploadfileblock);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_deletefileinfo, this, &Widget::slot_deletefile);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_renamefileinfo, this, &Widget::slot_renamefile);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_filesync, this, &Widget::slot_filesync);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_downloadfileinfo, this, &Widget::slot_downloadfileinfo);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_downloadfileblock, this, &Widget::slot_downloadfileblock);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_chat, this, &Widget::slot_chat);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_onlineusers, this, &Widget::slot_onlineusers);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_privatechat, this, &Widget::slot_privatechat);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_privatehistory, this, &Widget::slot_privatehistory);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_transfercontrol, this, &Widget::slot_transfercontrol);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_connectionStateChanged, this, &Widget::slot_connectionStateChanged);
    connect(static_cast<TCPKernel*>(m_kernel), &TCPKernel::signal_connectionStateChanged, m_login, &Login::slot_connectionStateChanged);

    connect(m_transferPanel, &TransferPanel::pauseRequested, this, &Widget::onTransferPauseRequested);
    connect(m_transferPanel, &TransferPanel::resumeRequested, this, &Widget::onTransferResumeRequested);
    connect(m_transferPanel, &TransferPanel::cancelRequested, this, &Widget::onTransferCancelRequested);

    if (m_kernel->connect()) {
        m_connectionBadge->setText(QStringLiteral("已连接服务端"));
        m_login->slot_connectionStateChanged(true, QStringLiteral("TLS connected"));
        appendStatus(QStringLiteral("客户端已连接到服务端（TLS）。"));
    } else {
        m_connectionBadge->setText(QStringLiteral("连接失败"));
        m_login->slot_connectionStateChanged(false, QStringLiteral("连接失败"));
        appendStatus(QStringLiteral("客户端连接服务端失败。"));
    }

    syncTransferSummary();
    m_login->show();
}

Widget::~Widget()
{
    if (m_kernel) {
        if (auto* tcpKernel = dynamic_cast<TCPKernel*>(m_kernel)) {
            QObject::disconnect(tcpKernel, nullptr, this, nullptr);
        }
        m_kernel->disconnect("widget destroyed");
    }

    for (auto* task : m_transferTasks) {
        closeTaskFile(task);
        delete task;
    }
    m_transferTasks.clear();

    delete m_transferPanel;
    delete m_chat;
    delete m_login;
}

void Widget::slot_login(STRU_LOGIN_RS* response)
{
    QString result = QStringLiteral("用户不存在");
    if (response->m_szResult == _login_passworderr) {
        result = QStringLiteral("密码错误");
    }

    if (response->m_szResult == _login_success) {
        m_login->hide();
        show();
        m_userId = response->m_userId;
        m_userLabel->setText(QStringLiteral("当前用户：%1").arg(m_login->currentUserName()));
        m_chat->setCurrentUser(m_userId, m_login->currentUserName());
        appendStatus(QStringLiteral("登录成功，用户ID=%1。").arg(m_userId));
        refreshFileList();
        return;
    }

    QMessageBox::information(this, QStringLiteral("登录"), result);
}

void Widget::slot_getfilelist(STRU_GETFILELIST_RS* response)
{
    if (!response) {
        return;
    }

    for (int i = 0; i < response->m_FileNum; ++i) {
        addOrUpdateFileRow(QString::fromLocal8Bit(response->m_aryInfo[i].m_szFileName),
                           response->m_aryInfo[i].m_fileSize,
                           QString::fromLocal8Bit(response->m_aryInfo[i].m_szFileDateTime),
                           QString::fromLocal8Bit(response->m_aryInfo[i].m_szFileMD5),
                           response->m_aryInfo[i].m_fileState);
    }
}

void Widget::slot_uploadfileinfo(STRU_UPLOADFILEINFO_RS* response)
{
    if (!response) {
        return;
    }

    TransferTask* task = findTaskByUploadResponse(QString::fromLocal8Bit(response->m_szFileName),
                                                  QString::fromLocal8Bit(response->m_szFileMD5));
    if (!task) {
        return;
    }

    task->fileId = response->m_fileId;
    task->resumePos = response->m_pos;
    task->acknowledgedBytes = response->m_pos;
    task->detailText = QStringLiteral("服务端已接受上传任务");

    if (response->m_szResult == _fileinfo_busy) {
        markTaskPaused(task, QStringLiteral("文件正在被其他终端上传，请稍后再试"));
        return;
    }

    if (response->m_szResult == _fileinfo_isuploaded || response->m_szResult == _fileinfo_speedtransfer) {
        addOrUpdateFileRow(task->displayName, task->totalBytes, currentTimestamp(), task->md5);
        markTaskCompleted(task, response->m_szResult == _fileinfo_speedtransfer
            ? QStringLiteral("秒传成功")
            : QStringLiteral("文件已存在，无需重复上传"));
        refreshFileList();
        return;
    }

    if (!openTaskFileForUpload(task, response->m_pos)) {
        markTaskFailed(task, QStringLiteral("无法读取本地文件"));
        return;
    }

    task->state = TransferTask::Running;
    task->transferredBytes = response->m_pos;
    task->acknowledgedBytes = response->m_pos;
    task->detailText = response->m_pos > 0
        ? QStringLiteral("从 %1 处继续上传").arg(formatSize(response->m_pos))
        : QStringLiteral("开始上传");
    syncTransferPanelTask(task);
    ensureUploadPump();
}

void Widget::slot_uploadfileblock(STRU_UPLOADFILEBLOCK_RS* response)
{
    if (!response) {
        return;
    }

    TransferTask* task = findTaskByFileId(TransferTask::Upload, response->m_fileId);
    if (!task) {
        return;
    }

    const qint64 acknowledgedDelta = qMax<qint64>(0, response->m_pos - task->acknowledgedBytes);
    task->acknowledgedBytes = qMax(task->acknowledgedBytes, response->m_pos);
    task->transferredBytes = qMax(task->transferredBytes, response->m_pos);
    task->totalBytes = qMax(task->totalBytes, response->m_fileSize);
    task->recentBytes += acknowledgedDelta;
    task->detailText = QStringLiteral("已上传 %1").arg(formatSize(task->transferredBytes));

    if (response->m_szResult == _transfer_result_finished || task->transferredBytes >= task->totalBytes) {
        addOrUpdateFileRow(task->displayName, task->totalBytes, currentTimestamp(), task->md5);
        markTaskCompleted(task, QStringLiteral("上传完成"));
        refreshFileList();
        return;
    }

    if (task->state == TransferTask::AwaitingServer) {
        task->state = TransferTask::Running;
    }
    syncTransferPanelTask(task);
}

void Widget::slot_deletefile(STRU_DELETEFILE_RS* response)
{
    if (!response) {
        return;
    }

    const QString md5 = QString::fromLocal8Bit(response->m_szFileMD5);
    if (response->m_szResult == _delete_success) {
        appendStatus(QStringLiteral("文件已删除。"));
        refreshFileList();
        return;
    }

    QMessageBox::information(this, QStringLiteral("删除"), QStringLiteral("文件不存在或删除失败。"));
}

void Widget::slot_renamefile(STRU_RENAMEFILE_RS* response)
{
    if (!response) {
        return;
    }

    if (response->m_szResult == _rename_success) {
        appendStatus(QStringLiteral("文件已重命名为：%1").arg(QString::fromLocal8Bit(response->m_szNewFileName)));
        refreshFileList();
        return;
    }

    QMessageBox::information(this, QStringLiteral("重命名"), QStringLiteral("重命名失败，文件不存在或名称无效。"));
}

void Widget::slot_filesync(STRU_FILESYNC_RS* response)
{
    if (!response || response->m_userId != m_userId) {
        return;
    }

    QString actionText;
    switch (response->m_action) {
    case _filesync_action_upload_started:
        actionText = QStringLiteral("其他终端开始上传");
        break;
    case _filesync_action_upload_completed:
        actionText = QStringLiteral("其他终端上传完成");
        break;
    case _filesync_action_delete:
        actionText = QStringLiteral("其他终端删除了文件");
        break;
    case _filesync_action_rename:
        actionText = QStringLiteral("其他终端重命名了文件");
        break;
    case _filesync_action_upload_paused:
        actionText = QStringLiteral("其他终端暂停了上传");
        break;
    case _filesync_action_upload_cancelled:
        actionText = QStringLiteral("其他终端取消了上传");
        break;
    default:
        actionText = QStringLiteral("其他终端更新了文件列表");
        break;
    }

    appendStatus(QStringLiteral("%1：%2")
                     .arg(actionText, QString::fromLocal8Bit(response->m_szFileName)));
    refreshFileList();
}

void Widget::slot_downloadfileinfo(STRU_DOWNLOADFILEINFO_RS* response)
{
    if (!response) {
        return;
    }

    TransferTask* task = nullptr;
    if (response->m_fileId > 0) {
        task = findTaskByFileId(TransferTask::Download, response->m_fileId);
    }
    if (!task && qstrlen(response->m_szFileMD5) > 0) {
        const QString md5 = QString::fromLocal8Bit(response->m_szFileMD5);
        task = m_transferTasks.value(makeTaskKey(TransferTask::Download, md5), nullptr);
    }
    if (!task || task->state != TransferTask::AwaitingServer) {
        return;
    }

    if (response->m_szResult == _fileinfo_busy) {
        markTaskPaused(task, QStringLiteral("文件正在被其他终端上传，请稍后再试"));
        return;
    }

    if (response->m_szResult != _fileinfo_normal) {
        markTaskFailed(task, QStringLiteral("服务端未找到该文件"));
        return;
    }

    task->fileId = response->m_fileId;
    task->transferredBytes = response->m_pos;
    task->state = TransferTask::Running;
    task->detailText = response->m_pos > 0
        ? QStringLiteral("从 %1 处继续下载").arg(formatSize(response->m_pos))
        : QStringLiteral("开始下载");
    syncTransferPanelTask(task);

    STRU_DOWNLOADFILEBLOCK_RQ request;
    request.m_fileId = response->m_fileId;
    request.m_fileNum = 0;
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Widget::slot_downloadfileblock(STRU_DOWNLOADFILEBLOCK_RS* response)
{
    if (!response) {
        return;
    }

    TransferTask* task = findTaskByFileId(TransferTask::Download, response->m_fileId);
    if (!task || task->state == TransferTask::Paused || task->state == TransferTask::Cancelled) {
        return;
    }

    if (!task->file) {
        if (!openTaskFileForDownload(task, true)) {
            markTaskFailed(task, QStringLiteral("无法写入本地文件"));
            return;
        }
    }

    if (response->m_fileNum > 0) {
        const qint64 written = task->file->write(response->m_szFileContent, response->m_fileNum);
        if (written != response->m_fileNum) {
            markTaskFailed(task, QStringLiteral("写入本地文件失败"));
            return;
        }
        task->transferredBytes += written;
        task->recentBytes += written;
        task->detailText = QStringLiteral("已下载 %1").arg(formatSize(task->transferredBytes));
        syncTransferPanelTask(task);
    }

    if (response->m_fileNum == 0 || task->transferredBytes >= task->totalBytes) {
        markTaskCompleted(task, QStringLiteral("下载完成"));
        return;
    }

    if (task->state == TransferTask::Running) {
        STRU_DOWNLOADFILEBLOCK_RQ request;
        request.m_fileId = response->m_fileId;
        request.m_fileNum = 0;
        m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
    }
}

void Widget::slot_chat(STRU_CHAT_RS* response)
{
    if (!response) {
        return;
    }
    m_chat->appendIncomingMessage(response);
}

void Widget::slot_onlineusers(STRU_ONLINE_USERS_RS* response)
{
    if (!response) {
        return;
    }
    m_chat->updateOnlineUsers(response);
}

void Widget::slot_privatechat(STRU_PRIVATE_CHAT_RS* response)
{
    if (!response) {
        return;
    }
    m_chat->appendPrivateMessage(response);
}

void Widget::slot_privatehistory(STRU_PRIVATE_HISTORY_RS* response)
{
    if (!response) {
        return;
    }
    m_chat->loadPrivateHistory(response);
}

void Widget::slot_transfercontrol(STRU_TRANSFERCONTROL_RS* response)
{
    if (!response) {
        return;
    }

    TransferTask* task = nullptr;
    if (response->m_fileId > 0) {
        task = findTaskByFileId(response->m_target == _transfer_target_download ? TransferTask::Download : TransferTask::Upload,
                                response->m_fileId);
    }
    if (!task && qstrlen(response->m_szFileMD5) > 0) {
        const QString md5 = QString::fromLocal8Bit(response->m_szFileMD5);
        task = m_transferTasks.value(makeTaskKey(response->m_target == _transfer_target_download ? TransferTask::Download : TransferTask::Upload, md5), nullptr);
    }
    if (!task) {
        return;
    }

    if (response->m_action == _transfer_action_pause && response->m_szResult == _transfer_result_running) {
        task->detailText = QStringLiteral("服务端已暂停会话");
        syncTransferPanelTask(task);
        return;
    }

    if (response->m_action == _transfer_action_cancel && response->m_szResult == _transfer_result_running) {
        task->detailText = QStringLiteral("服务端已取消会话");
        syncTransferPanelTask(task);
    }
}

void Widget::slot_connectionStateChanged(bool connected, const QString& reason)
{
    m_connectionBadge->setText(connected ? QStringLiteral("已连接服务端") : QStringLiteral("连接中断"));
    if (connected) {
        appendStatus(QStringLiteral("网络连接已建立：%1").arg(reason));
        return;
    }

    appendStatus(QStringLiteral("网络连接中断：%1").arg(reason));
    for (auto* task : m_transferTasks) {
        if (!task) {
            continue;
        }
        if (task->state == TransferTask::Running || task->state == TransferTask::AwaitingServer) {
            markTaskPaused(task, QStringLiteral("连接中断，可继续传输"));
        }
    }
}

void Widget::onUploadClicked()
{
    if (m_userId <= 0) {
        QMessageBox::information(this, QStringLiteral("上传"), QStringLiteral("请先登录。"));
        return;
    }

    if (!m_kernel->isConnected() && !m_kernel->connect()) {
        QMessageBox::warning(this, QStringLiteral("上传"), QStringLiteral("当前未连接服务端。"));
        return;
    }

    const QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("选择上传文件"));
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    const qint64 size = fileInfo.size();
    const QString md5 = computeFileMd5(filePath);
    if (md5.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("上传"), QStringLiteral("无法计算文件摘要。"));
        return;
    }

    const QString taskKey = makeTaskKey(TransferTask::Upload, md5);
    if (m_transferTasks.contains(taskKey)) {
        auto* existing = m_transferTasks.value(taskKey);
        if (existing && existing->state != TransferTask::Completed && existing->state != TransferTask::Cancelled) {
            m_transferPanel->show();
            m_transferPanel->raise();
            m_transferPanel->activateWindow();
            return;
        }
    }

    auto* task = new TransferTask;
    task->key = taskKey;
    task->displayName = fileInfo.fileName();
    task->localPath = filePath;
    task->md5 = md5;
    task->totalBytes = size;
    task->direction = TransferTask::Upload;
    task->state = TransferTask::Queued;
    task->detailText = QStringLiteral("等待上传");
    m_transferTasks.insert(taskKey, task);
    syncTransferPanelTask(task);
    startUploadHandshake(task);

    m_transferPanel->show();
    m_transferPanel->raise();
    m_transferPanel->activateWindow();
}

void Widget::onDownloadClicked()
{
    if (m_userId <= 0) {
        QMessageBox::information(this, QStringLiteral("下载"), QStringLiteral("请先登录。"));
        return;
    }

    if (!m_kernel->isConnected() && !m_kernel->connect()) {
        QMessageBox::warning(this, QStringLiteral("下载"), QStringLiteral("当前未连接服务端。"));
        return;
    }

    const QString md5 = selectedFileMd5();
    const QString fileName = selectedFileName();
    const qint64 fileSize = selectedFileSize();
    const int fileState = selectedFileState();
    if (md5.isEmpty() || fileName.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("下载"), QStringLiteral("请先选中文件。"));
        return;
    }
    if (fileState != _filestate_ready) {
        QMessageBox::warning(this, QStringLiteral("下载"), QStringLiteral("该文件未通过完整性检测，当前禁止下载。"));
        return;
    }

    const QString savePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存文件"), fileName);
    if (savePath.isEmpty()) {
        return;
    }

    const QString taskKey = makeTaskKey(TransferTask::Download, md5);
    auto* task = m_transferTasks.value(taskKey, nullptr);
    if (!task) {
        task = new TransferTask;
        task->key = taskKey;
        task->direction = TransferTask::Download;
        task->md5 = md5;
        m_transferTasks.insert(taskKey, task);
    } else {
        closeTaskFile(task);
    }

    task->displayName = fileName;
    task->localPath = savePath;
    task->totalBytes = fileSize;
    task->detailText = QStringLiteral("等待下载");
    task->state = TransferTask::Queued;
    task->fileId = 0;
    task->acknowledgedBytes = 0;
    task->bytesPerSecond = 0;
    task->recentBytes = 0;

    const QFileInfo existingInfo(savePath);
    const bool resumeExistingFile = existingInfo.exists() && existingInfo.size() > 0 && existingInfo.size() < fileSize;
    if (!resumeExistingFile && existingInfo.exists()) {
        QFile::remove(savePath);
    }

    syncTransferPanelTask(task);
    startDownloadHandshake(task, resumeExistingFile);

    m_transferPanel->show();
    m_transferPanel->raise();
    m_transferPanel->activateWindow();
}

void Widget::onDeleteClicked()
{
    const QString md5 = selectedFileMd5();
    if (md5.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("删除"), QStringLiteral("请先选中文件。"));
        return;
    }

    STRU_DELETEFILE_RQ request;
    request.m_userId = m_userId;
    qstrncpy(request.m_szFileMD5, md5.toLocal8Bit().constData(), MAXSIZE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Widget::onChatClicked()
{
    m_chat->show();
    m_chat->raise();
    m_chat->activateWindow();
}

void Widget::onRefreshClicked()
{
    refreshFileList();
}

void Widget::onShareClicked()
{
    if (!selectedFileMd5().isEmpty() && selectedFileState() != _filestate_ready) {
        QMessageBox::warning(this, QStringLiteral("分享"), QStringLiteral("该文件未通过完整性检测，当前禁止分享。"));
        return;
    }
    QMessageBox::information(this, QStringLiteral("分享"), QStringLiteral("分享功能保留入口，后续可继续扩展提取码与链接生成。"));
}

void Widget::onSelectionChanged()
{
    const bool hasSelection = !selectedFileMd5().isEmpty();
    const bool isReady = selectedFileState() == _filestate_ready;
    m_downloadButton->setEnabled(hasSelection && isReady);
    m_deleteButton->setEnabled(hasSelection);
    m_shareButton->setEnabled(hasSelection && isReady);
}

void Widget::onFileTableContextMenuRequested(const QPoint& pos)
{
    if (!selectRowAtViewportPos(pos)) {
        return;
    }

    const QString md5 = selectedFileMd5();
    if (md5.isEmpty()) {
        return;
    }

    const int fileState = selectedFileState();
    QMenu menu(this);
    QAction* downloadAction = menu.addAction(QStringLiteral("下载"), this, &Widget::onDownloadClicked);
    QAction* deleteAction = menu.addAction(QStringLiteral("删除"), this, &Widget::onDeleteClicked);
    QAction* renameAction = menu.addAction(QStringLiteral("修改名字"), this, &Widget::requestRenameSelectedFile);

    const bool isReady = fileState == _filestate_ready;
    downloadAction->setEnabled(isReady);
    deleteAction->setEnabled(true);
    renameAction->setEnabled(isReady);

    if (fileState == _filestate_incomplete || fileState == _filestate_abnormal) {
        menu.addAction(QStringLiteral("继续上传"), this, &Widget::requestResumeSelectedUpload);
    }

    if (menu.actions().isEmpty()) {
        return;
    }

    menu.exec(m_fileTable->viewport()->mapToGlobal(pos));
}

void Widget::onTransfersClicked()
{
    m_transferPanel->show();
    m_transferPanel->raise();
    m_transferPanel->activateWindow();
}

void Widget::onTransferPauseRequested(const QString& taskKey)
{
    auto* task = m_transferTasks.value(taskKey, nullptr);
    if (!task || (task->state != TransferTask::Running && task->state != TransferTask::AwaitingServer)) {
        return;
    }

    if (task->direction == TransferTask::Upload) {
        sendTransferControl(task, _transfer_action_pause);
        stopUploadPumpIfIdle();
    } else if (task->direction == TransferTask::Download) {
        sendTransferControl(task, _transfer_action_pause);
    }

    markTaskPaused(task, QStringLiteral("已暂停，可继续传输"));
}

void Widget::onTransferResumeRequested(const QString& taskKey)
{
    auto* task = m_transferTasks.value(taskKey, nullptr);
    if (!task || (task->state != TransferTask::Paused && task->state != TransferTask::Failed && task->state != TransferTask::Cancelled)) {
        return;
    }

    if (!m_kernel->isConnected() && !m_kernel->connect()) {
        QMessageBox::warning(this, QStringLiteral("继续传输"), QStringLiteral("无法连接到服务端。"));
        return;
    }

    if (task->direction == TransferTask::Upload) {
        startUploadHandshake(task);
    } else {
        startDownloadHandshake(task, true);
    }
}

void Widget::onTransferCancelRequested(const QString& taskKey)
{
    auto* task = m_transferTasks.value(taskKey, nullptr);
    if (!task) {
        return;
    }

    sendTransferControl(task, _transfer_action_cancel);
    if (task->direction == TransferTask::Download && !task->localPath.isEmpty()) {
        closeTaskFile(task);
        QFile::remove(task->localPath);
    }
    markTaskCancelled(task, QStringLiteral("已取消传输"));
}

void Widget::processUploadPump()
{
    bool hasRunningUpload = false;
    for (auto* task : m_transferTasks) {
        if (!task || task->direction != TransferTask::Upload || task->state != TransferTask::Running || !task->file) {
            continue;
        }

        hasRunningUpload = true;
        int chunkBudget = 4;
        while (chunkBudget-- > 0 && task->file && !task->file->atEnd() && task->state == TransferTask::Running) {
            const QByteArray block = task->file->read(ONE_PAGE);
            if (block.isEmpty()) {
                break;
            }

            STRU_UPLOADFILEBLOCK_RQ request;
            request.m_fileId = task->fileId;
            request.m_fileNum = block.size();
            memcpy(request.m_szFileContent, block.constData(), block.size());

            if (!m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request))) {
                task->file->seek(task->file->pos() - block.size());
                markTaskPaused(task, QStringLiteral("网络繁忙，已暂停上传"));
                break;
            }

            task->transferredBytes = qMax(task->transferredBytes, task->file->pos());
            task->detailText = QStringLiteral("正在上传 %1").arg(formatSize(task->transferredBytes));
        }

        syncTransferPanelTask(task);
    }

    if (!hasRunningUpload) {
        m_uploadPumpTimer.stop();
    }
}

void Widget::updateTransferRates()
{
    for (auto* task : m_transferTasks) {
        if (!task) {
            continue;
        }
        task->bytesPerSecond = task->recentBytes;
        task->recentBytes = 0;
        syncTransferPanelTask(task);
    }
}

void Widget::buildUi()
{
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(18);

    auto* sidePanel = new QWidget(this);
    sidePanel->setObjectName(QStringLiteral("sidePanel"));
    auto* sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setContentsMargins(18, 18, 18, 18);
    sideLayout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Jaeger Disk"), sidePanel);
    title->setObjectName(QStringLiteral("sideTitle"));
    auto* subtitle = new QLabel(QStringLiteral("上传、下载、聊天与文件管理"), sidePanel);
    subtitle->setWordWrap(true);
    subtitle->setObjectName(QStringLiteral("sideSubtitle"));
    sideLayout->addWidget(title);
    sideLayout->addWidget(subtitle);

    m_chatButton = new QPushButton(QStringLiteral("打开聊天"), sidePanel);
    m_uploadButton = new QPushButton(QStringLiteral("上传文件"), sidePanel);
    m_downloadButton = new QPushButton(QStringLiteral("下载文件"), sidePanel);
    m_transfersButton = new QPushButton(QStringLiteral("传输管理"), sidePanel);
    m_deleteButton = new QPushButton(QStringLiteral("删除文件"), sidePanel);
    m_shareButton = new QPushButton(QStringLiteral("分享入口"), sidePanel);
    m_refreshButton = new QPushButton(QStringLiteral("刷新列表"), sidePanel);

    connect(m_chatButton, &QPushButton::clicked, this, &Widget::onChatClicked);
    connect(m_uploadButton, &QPushButton::clicked, this, &Widget::onUploadClicked);
    connect(m_downloadButton, &QPushButton::clicked, this, &Widget::onDownloadClicked);
    connect(m_transfersButton, &QPushButton::clicked, this, &Widget::onTransfersClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &Widget::onDeleteClicked);
    connect(m_shareButton, &QPushButton::clicked, this, &Widget::onShareClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &Widget::onRefreshClicked);

    sideLayout->addSpacing(10);
    sideLayout->addWidget(m_chatButton);
    sideLayout->addWidget(m_uploadButton);
    sideLayout->addWidget(m_downloadButton);
    sideLayout->addWidget(m_transfersButton);
    sideLayout->addWidget(m_deleteButton);
    sideLayout->addWidget(m_shareButton);
    sideLayout->addWidget(m_refreshButton);
    sideLayout->addStretch();

    auto* mainPanel = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(mainPanel);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(14);

    auto* topBar = new QWidget(mainPanel);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    m_connectionBadge = new QLabel(QStringLiteral("未连接"), topBar);
    m_connectionBadge->setObjectName(QStringLiteral("connectionBadge"));
    m_userLabel = new QLabel(QStringLiteral("当前用户：未登录"), topBar);
    topLayout->addWidget(m_connectionBadge);
    topLayout->addStretch();
    topLayout->addWidget(m_userLabel);

    m_fileTable = new QTableWidget(0, 5, mainPanel);
    m_fileTable->setHorizontalHeaderLabels({QStringLiteral("文件名"), QStringLiteral("大小"), QStringLiteral("上传时间"), QStringLiteral("状态"), QStringLiteral("MD5")});
    m_fileTable->horizontalHeader()->setStretchLastSection(false);
    m_fileTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_fileTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_fileTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_fileTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_fileTable->setColumnHidden(4, true);
    m_fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fileTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_fileTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_fileTable, &QTableWidget::itemSelectionChanged, this, &Widget::onSelectionChanged);
    connect(m_fileTable, &QWidget::customContextMenuRequested, this, &Widget::onFileTableContextMenuRequested);

    m_statusLog = new QTextEdit(mainPanel);
    m_statusLog->setReadOnly(true);
    m_statusLog->setMaximumHeight(160);

    mainLayout->addWidget(topBar);
    mainLayout->addWidget(m_fileTable, 1);
    mainLayout->addWidget(m_statusLog);

    rootLayout->addWidget(sidePanel, 3);
    rootLayout->addWidget(mainPanel, 7);

    m_downloadButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    hide();
}

void Widget::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "QWidget#diskMainWindow { background: #eef3f7; color: #16324f; }"
        "QLabel { background: transparent; }"
        "QWidget#sidePanel { background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #16324f, stop:1 #2a6f97); border-radius: 24px; }"
        "QLabel#sideTitle { color: white; font-size: 28px; font-weight: 700; }"
        "QLabel#sideSubtitle { color: rgba(255,255,255,0.76); font-size: 14px; }"
        "QLabel#connectionBadge { background: #d9f0e3; color: #14532d; border-radius: 12px; padding: 6px 12px; font-weight: 600; }"
        "QPushButton { min-height: 42px; border-radius: 14px; border: none; background: white; color: #16324f; font-weight: 600; }"
        "QPushButton:hover { background: #dbe7f0; }"
        "QPushButton:disabled { background: #b9c8d3; color: #5f7385; }"
        "QTableWidget { background: rgba(255,255,255,0.96); border-radius: 18px; border: 1px solid #d4dee7; gridline-color: #edf2f6; }"
        "QHeaderView::section { background: #dbe7f0; color: #16324f; border: none; padding: 10px; font-weight: 700; }"
        "QTextEdit { background: rgba(255,255,255,0.96); border-radius: 18px; border: 1px solid #d4dee7; padding: 10px; }"));
}

void Widget::appendStatus(const QString& message)
{
    m_statusLog->append(QStringLiteral("[%1] %2").arg(currentTimestamp(), message));
}

void Widget::refreshFileList()
{
    if (m_userId <= 0) {
        return;
    }
    m_fileTable->setRowCount(0);
    STRU_GETFILELIST_RQ request;
    request.m_userId = m_userId;
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Widget::addOrUpdateFileRow(const QString& fileName, qint64 fileSize, const QString& uploadTime, const QString& md5)
{
    addOrUpdateFileRow(fileName, fileSize, uploadTime, md5, _filestate_ready);
}

void Widget::addOrUpdateFileRow(const QString& fileName, qint64 fileSize, const QString& uploadTime, const QString& md5, int fileState)
{
    for (int row = 0; row < m_fileTable->rowCount(); ++row) {
        auto* item = m_fileTable->item(row, 4);
        if (item && item->text() == md5) {
            m_fileTable->item(row, 0)->setText(fileName);
            m_fileTable->item(row, 1)->setText(formatSize(fileSize));
            m_fileTable->item(row, 2)->setText(uploadTime);
            m_fileTable->item(row, 3)->setText(fileStateText(fileState));
            m_fileTable->item(row, 3)->setData(Qt::UserRole, fileState);
            applyFileStateRowStyle(row, fileState);
            return;
        }
    }

    const int row = m_fileTable->rowCount();
    m_fileTable->insertRow(row);
    auto* nameItem = new QTableWidgetItem(QIcon(QStringLiteral(":/app/file.png")), fileName);
    auto* sizeItem = new QTableWidgetItem(formatSize(fileSize));
    sizeItem->setData(Qt::UserRole, fileSize);
    auto* timeItem = new QTableWidgetItem(uploadTime);
    auto* stateItem = new QTableWidgetItem(fileStateText(fileState));
    stateItem->setData(Qt::UserRole, fileState);
    auto* md5Item = new QTableWidgetItem(md5);
    m_fileTable->setItem(row, 0, nameItem);
    m_fileTable->setItem(row, 1, sizeItem);
    m_fileTable->setItem(row, 2, timeItem);
    m_fileTable->setItem(row, 3, stateItem);
    m_fileTable->setItem(row, 4, md5Item);
    applyFileStateRowStyle(row, fileState);
}

void Widget::applyFileStateRowStyle(int row, int fileState)
{
    QColor backgroundColor = QColor(QStringLiteral("#eef8f0"));
    QColor foregroundColor = QColor(QStringLiteral("#1f5130"));

    switch (fileState) {
    case _filestate_ready:
        backgroundColor = QColor(QStringLiteral("#eef8f0"));
        foregroundColor = QColor(QStringLiteral("#1f5130"));
        break;
    case _filestate_incomplete:
    case _filestate_uploading:
        backgroundColor = QColor(QStringLiteral("#fff7e6"));
        foregroundColor = QColor(QStringLiteral("#8a5a00"));
        break;
    case _filestate_abnormal:
        backgroundColor = QColor(QStringLiteral("#fdecec"));
        foregroundColor = QColor(QStringLiteral("#8f1d1d"));
        break;
    default:
        backgroundColor = QColor(Qt::white);
        foregroundColor = QColor(QStringLiteral("#16324f"));
        break;
    }

    for (int column = 0; column < m_fileTable->columnCount(); ++column) {
        auto* cell = m_fileTable->item(row, column);
        if (!cell) {
            continue;
        }
        cell->setBackground(QBrush(backgroundColor));
        cell->setForeground(QBrush(foregroundColor));
    }
}

QString Widget::selectedFileMd5() const
{
    const auto items = m_fileTable->selectedItems();
    if (items.isEmpty()) {
        return {};
    }
    const int row = items.first()->row();
    auto* item = m_fileTable->item(row, 4);
    return item ? item->text() : QString();
}

QString Widget::selectedFileName() const
{
    const auto items = m_fileTable->selectedItems();
    if (items.isEmpty()) {
        return {};
    }
    const int row = items.first()->row();
    auto* item = m_fileTable->item(row, 0);
    return item ? item->text() : QString();
}

qint64 Widget::selectedFileSize() const
{
    const auto items = m_fileTable->selectedItems();
    if (items.isEmpty()) {
        return 0;
    }
    const int row = items.first()->row();
    auto* item = m_fileTable->item(row, 1);
    return item ? item->data(Qt::UserRole).toLongLong() : 0;
}

int Widget::selectedFileState() const
{
    const auto items = m_fileTable->selectedItems();
    if (items.isEmpty()) {
        return _filestate_ready;
    }
    const int row = items.first()->row();
    auto* item = m_fileTable->item(row, 3);
    return item ? item->data(Qt::UserRole).toInt() : _filestate_ready;
}

bool Widget::selectRowAtViewportPos(const QPoint& pos)
{
    if (!m_fileTable) {
        return false;
    }

    const QModelIndex index = m_fileTable->indexAt(pos);
    if (!index.isValid()) {
        return false;
    }

    m_fileTable->selectRow(index.row());
    return true;
}

void Widget::requestRenameSelectedFile()
{
    const QString md5 = selectedFileMd5();
    const QString oldName = selectedFileName();
    const int fileState = selectedFileState();

    if (md5.isEmpty() || oldName.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("重命名"), QStringLiteral("请先选中文件。"));
        return;
    }
    if (fileState != _filestate_ready) {
        QMessageBox::warning(this, QStringLiteral("重命名"), QStringLiteral("仅完整文件支持重命名。"));
        return;
    }

    bool ok = false;
    const QString newName = QInputDialog::getText(this,
                                                  QStringLiteral("修改名字"),
                                                  QStringLiteral("请输入新的文件名："),
                                                  QLineEdit::Normal,
                                                  oldName,
                                                  &ok).trimmed();
    if (!ok) {
        return;
    }
    if (newName.isEmpty() || newName == oldName) {
        return;
    }

    STRU_RENAMEFILE_RQ request;
    request.m_userId = m_userId;
    qstrncpy(request.m_szFileMD5, md5.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_szNewFileName, newName.toLocal8Bit().constData(), NAMESIZE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Widget::requestResumeSelectedUpload()
{
    const QString md5 = selectedFileMd5();
    const QString remoteName = selectedFileName();
    const qint64 remoteSize = selectedFileSize();
    const int fileState = selectedFileState();

    if (md5.isEmpty() || remoteName.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("继续上传"), QStringLiteral("请先选中文件。"));
        return;
    }
    if (fileState != _filestate_incomplete && fileState != _filestate_abnormal) {
        QMessageBox::information(this, QStringLiteral("继续上传"), QStringLiteral("当前文件无需继续上传。"));
        return;
    }
    if (!m_kernel->isConnected() && !m_kernel->connect()) {
        QMessageBox::warning(this, QStringLiteral("继续上传"), QStringLiteral("当前未连接服务端。"));
        return;
    }

    TransferTask* task = m_transferTasks.value(makeTaskKey(TransferTask::Upload, md5), nullptr);
    if (task && !task->localPath.isEmpty()) {
        QFileInfo localInfo(task->localPath);
        if (localInfo.exists() && computeFileMd5(task->localPath) == md5) {
            task->displayName = remoteName;
            task->totalBytes = localInfo.size();
            startUploadHandshake(task);
            m_transferPanel->show();
            m_transferPanel->raise();
            m_transferPanel->activateWindow();
            return;
        }
    }

    const QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("选择本地文件以继续上传"), remoteName);
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    const QString localMd5 = computeFileMd5(filePath);
    if (localMd5.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("继续上传"), QStringLiteral("无法计算本地文件摘要。"));
        return;
    }
    if (localMd5 != md5) {
        QMessageBox::warning(this, QStringLiteral("继续上传"), QStringLiteral("所选文件与服务器记录不一致，无法继续上传。"));
        return;
    }

    const QString taskKey = makeTaskKey(TransferTask::Upload, md5);
    if (!task) {
        task = new TransferTask;
        task->key = taskKey;
        task->direction = TransferTask::Upload;
        task->md5 = md5;
        m_transferTasks.insert(taskKey, task);
    } else {
        closeTaskFile(task);
    }

    task->displayName = remoteName;
    task->localPath = filePath;
    task->totalBytes = fileInfo.size();
    task->detailText = QStringLiteral("准备继续上传");
    task->state = TransferTask::Queued;
    task->bytesPerSecond = 0;
    task->recentBytes = 0;
    task->acknowledgedBytes = 0;

    syncTransferPanelTask(task);
    startUploadHandshake(task);

    m_transferPanel->show();
    m_transferPanel->raise();
    m_transferPanel->activateWindow();
}

QString Widget::formatSize(qint64 bytes) const
{
    if (bytes < 1024) {
        return QStringLiteral("%1 B").arg(bytes);
    }
    if (bytes < 1024 * 1024) {
        return QStringLiteral("%1 KB").arg(QString::number(bytes / 1024.0, 'f', 1));
    }
    if (bytes < 1024ll * 1024ll * 1024ll) {
        return QStringLiteral("%1 MB").arg(QString::number(bytes / 1024.0 / 1024.0, 'f', 1));
    }
    return QStringLiteral("%1 GB").arg(QString::number(bytes / 1024.0 / 1024.0 / 1024.0, 'f', 1));
}

QString Widget::fileStateText(int fileState) const
{
    switch (fileState) {
    case _filestate_ready:
        return QStringLiteral("完整");
    case _filestate_uploading:
        return QStringLiteral("上传中");
    case _filestate_incomplete:
        return QStringLiteral("未完成");
    case _filestate_abnormal:
        return QStringLiteral("异常");
    default:
        return QStringLiteral("未知");
    }
}

QString Widget::computeFileMd5(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    MD5 md5;
    char buffer[8192];
    while (true) {
        const qint64 length = file.read(buffer, sizeof(buffer));
        if (length <= 0) {
            break;
        }
        md5.update(reinterpret_cast<const md5_byte*>(buffer), static_cast<size_t>(length));
    }

    return QString::fromStdString(md5.toString());
}

QString Widget::makeTaskKey(TransferTask::Direction direction, const QString& md5) const
{
    return QStringLiteral("%1:%2")
        .arg(direction == TransferTask::Upload ? QStringLiteral("upload") : QStringLiteral("download"))
        .arg(md5);
}

TransferTaskSnapshot Widget::makeSnapshot(const TransferTask* task) const
{
    TransferTaskSnapshot snapshot;
    if (!task) {
        return snapshot;
    }

    snapshot.key = task->key;
    snapshot.displayName = task->displayName;
    snapshot.md5 = task->md5;
    snapshot.fileId = task->fileId;
    snapshot.totalBytes = task->totalBytes;
    snapshot.transferredBytes = task->transferredBytes;
    snapshot.bytesPerSecond = task->bytesPerSecond;
    snapshot.direction = task->direction == TransferTask::Upload ? TransferTaskSnapshot::Upload : TransferTaskSnapshot::Download;
    switch (task->state) {
    case TransferTask::Queued:
    case TransferTask::AwaitingServer:
        snapshot.state = TransferTaskSnapshot::Queued;
        break;
    case TransferTask::Running:
        snapshot.state = TransferTaskSnapshot::Running;
        break;
    case TransferTask::Paused:
        snapshot.state = TransferTaskSnapshot::Paused;
        break;
    case TransferTask::Completed:
        snapshot.state = TransferTaskSnapshot::Completed;
        break;
    case TransferTask::Failed:
        snapshot.state = TransferTaskSnapshot::Failed;
        break;
    case TransferTask::Cancelled:
        snapshot.state = TransferTaskSnapshot::Cancelled;
        break;
    }
    snapshot.detailText = task->detailText;
    return snapshot;
}

void Widget::syncTransferPanelTask(TransferTask* task)
{
    if (!task || !m_transferPanel) {
        return;
    }
    m_transferPanel->upsertTask(makeSnapshot(task));
    syncTransferSummary();
}

void Widget::syncTransferSummary()
{
    int running = 0;
    int paused = 0;
    int completed = 0;
    for (auto* task : m_transferTasks) {
        if (!task) {
            continue;
        }
        switch (task->state) {
        case TransferTask::AwaitingServer:
        case TransferTask::Running:
            ++running;
            break;
        case TransferTask::Paused:
            ++paused;
            break;
        case TransferTask::Completed:
            ++completed;
            break;
        default:
            break;
        }
    }
    m_transferPanel->setSummaryText(QStringLiteral("运行中 %1 项，暂停 %2 项，已完成 %3 项").arg(running).arg(paused).arg(completed));
}

void Widget::startUploadHandshake(TransferTask* task)
{
    if (!task) {
        return;
    }

    task->state = TransferTask::AwaitingServer;
    task->detailText = QStringLiteral("等待服务端分配上传会话");
    task->bytesPerSecond = 0;
    task->recentBytes = 0;
    task->acknowledgedBytes = 0;
    closeTaskFile(task);
    syncTransferPanelTask(task);

    STRU_UPLOADFILEINFO_RQ request;
    request.m_userid = m_userId;
    request.m_filesize = task->totalBytes;
    qstrncpy(request.m_szFileName, task->displayName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_szFileMD5, task->md5.toLocal8Bit().constData(), MAXSIZE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));

    appendStatus(QStringLiteral("准备上传文件：%1").arg(task->displayName));
}

void Widget::startDownloadHandshake(TransferTask* task, bool resumeExistingFile)
{
    if (!task) {
        return;
    }

    if (!openTaskFileForDownload(task, resumeExistingFile)) {
        markTaskFailed(task, QStringLiteral("无法打开本地保存文件"));
        return;
    }

    task->state = TransferTask::AwaitingServer;
    task->bytesPerSecond = 0;
    task->recentBytes = 0;
    task->detailText = task->transferredBytes > 0
        ? QStringLiteral("准备从 %1 处继续下载").arg(formatSize(task->transferredBytes))
        : QStringLiteral("等待服务端准备下载");
    syncTransferPanelTask(task);

    STRU_DOWNLOADFILEINFO_RQ request;
    request.m_userid = m_userId;
    request.m_filesize = task->totalBytes;
    request.m_pos = task->transferredBytes;
    qstrncpy(request.m_szFileName, task->displayName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(request.m_szFileMD5, task->md5.toLocal8Bit().constData(), MAXSIZE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));

    appendStatus(QStringLiteral("准备下载文件：%1").arg(task->displayName));
}

bool Widget::openTaskFileForUpload(TransferTask* task, qint64 startPos)
{
    if (!task) {
        return false;
    }

    closeTaskFile(task);
    auto* file = new QFile(task->localPath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
        return false;
    }
    if (startPos > 0 && !file->seek(startPos)) {
        file->close();
        delete file;
        return false;
    }
    task->file = file;
    task->transferredBytes = startPos;
    task->acknowledgedBytes = startPos;
    task->resumePos = startPos;
    return true;
}

bool Widget::openTaskFileForDownload(TransferTask* task, bool resumeExistingFile)
{
    if (!task) {
        return false;
    }

    closeTaskFile(task);
    auto* file = new QFile(task->localPath);
    QIODevice::OpenMode mode = QIODevice::WriteOnly;
    if (resumeExistingFile && file->exists()) {
        mode |= QIODevice::Append;
    } else {
        mode |= QIODevice::Truncate;
        task->transferredBytes = 0;
        task->acknowledgedBytes = 0;
    }

    if (!file->open(mode)) {
        delete file;
        return false;
    }
    if (resumeExistingFile) {
        task->transferredBytes = file->size();
        task->acknowledgedBytes = task->transferredBytes;
    }
    task->file = file;
    return true;
}

void Widget::closeTaskFile(TransferTask* task)
{
    if (!task || !task->file) {
        return;
    }

    task->file->close();
    delete task->file;
    task->file = nullptr;
}

void Widget::markTaskFailed(TransferTask* task, const QString& detailText)
{
    if (!task) {
        return;
    }
    closeTaskFile(task);
    task->state = TransferTask::Failed;
    task->bytesPerSecond = 0;
    task->detailText = detailText;
    appendStatus(QStringLiteral("%1 失败：%2").arg(task->displayName, detailText));
    syncTransferPanelTask(task);
    stopUploadPumpIfIdle();
}

void Widget::markTaskCompleted(TransferTask* task, const QString& detailText)
{
    if (!task) {
        return;
    }
    closeTaskFile(task);
    task->state = TransferTask::Completed;
    task->transferredBytes = task->totalBytes;
    task->bytesPerSecond = 0;
    task->detailText = detailText;
    appendStatus(QStringLiteral("%1：%2").arg(task->displayName, detailText));
    syncTransferPanelTask(task);
    stopUploadPumpIfIdle();
}

void Widget::markTaskCancelled(TransferTask* task, const QString& detailText)
{
    if (!task) {
        return;
    }
    closeTaskFile(task);
    task->state = TransferTask::Cancelled;
    task->bytesPerSecond = 0;
    task->recentBytes = 0;
    task->detailText = detailText;
    appendStatus(QStringLiteral("%1：%2").arg(task->displayName, detailText));
    syncTransferPanelTask(task);
    stopUploadPumpIfIdle();
}

void Widget::markTaskPaused(TransferTask* task, const QString& detailText)
{
    if (!task) {
        return;
    }
    closeTaskFile(task);
    task->state = TransferTask::Paused;
    task->bytesPerSecond = 0;
    task->recentBytes = 0;
    task->detailText = detailText;
    syncTransferPanelTask(task);
    stopUploadPumpIfIdle();
}

void Widget::ensureUploadPump()
{
    if (!m_uploadPumpTimer.isActive()) {
        m_uploadPumpTimer.start();
    }
}

void Widget::stopUploadPumpIfIdle()
{
    for (auto* task : m_transferTasks) {
        if (task && task->direction == TransferTask::Upload &&
            (task->state == TransferTask::Running || task->state == TransferTask::AwaitingServer)) {
            return;
        }
    }
    m_uploadPumpTimer.stop();
}

void Widget::sendTransferControl(TransferTask* task, char action)
{
    if (!task || task->fileId <= 0) {
        return;
    }

    STRU_TRANSFERCONTROL_RQ request;
    request.m_target = task->direction == TransferTask::Upload ? _transfer_target_upload : _transfer_target_download;
    request.m_action = action;
    request.m_fileId = task->fileId;
    qstrncpy(request.m_szFileMD5, task->md5.toLocal8Bit().constData(), MAXSIZE);
    m_kernel->sendData(reinterpret_cast<char*>(&request), sizeof(request));
}

void Widget::clearTaskIfTerminal(TransferTask* task)
{
    Q_UNUSED(task);
}

TransferTask* Widget::findTaskByUploadResponse(const QString& fileName, const QString& md5) const
{
    if (!md5.isEmpty()) {
        auto* task = m_transferTasks.value(makeTaskKey(TransferTask::Upload, md5), nullptr);
        if (task) {
            return task;
        }
    }

    for (auto* task : m_transferTasks) {
        if (!task || task->direction != TransferTask::Upload) {
            continue;
        }
        if (task->displayName == fileName &&
            (task->state == TransferTask::AwaitingServer || task->state == TransferTask::Running || task->state == TransferTask::Paused)) {
            return task;
        }
    }
    return nullptr;
}

TransferTask* Widget::findTaskByFileId(TransferTask::Direction direction, qint64 fileId) const
{
    for (auto* task : m_transferTasks) {
        if (task && task->direction == direction && task->fileId == fileId) {
            return task;
        }
    }
    return nullptr;
}
