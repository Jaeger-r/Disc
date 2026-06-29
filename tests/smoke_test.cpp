#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTemporaryDir>

#include "../Packdef.h"

namespace {
bool writePacket(QTcpSocket& socket, const void* payload, qint32 size)
{
    if (socket.write(reinterpret_cast<const char*>(&size), sizeof(size)) != sizeof(size)) {
        return false;
    }
    if (!socket.waitForBytesWritten(3000)) {
        return false;
    }
    if (socket.write(reinterpret_cast<const char*>(payload), size) != size) {
        return false;
    }
    return socket.waitForBytesWritten(3000);
}

QByteArray readPacket(QTcpSocket& socket, int timeoutMs = 5000)
{
    QElapsedTimer timer;
    timer.start();

    qint32 size = 0;
    while (socket.bytesAvailable() < static_cast<qint64>(sizeof(size))) {
        if (!socket.waitForReadyRead(qMax(1, timeoutMs - static_cast<int>(timer.elapsed())))) {
            return {};
        }
    }
    socket.read(reinterpret_cast<char*>(&size), sizeof(size));
    QByteArray payload;
    while (payload.size() < size) {
        if (socket.bytesAvailable() == 0) {
            if (!socket.waitForReadyRead(qMax(1, timeoutMs - static_cast<int>(timer.elapsed())))) {
                return {};
            }
        }
        payload += socket.read(size - payload.size());
    }
    return payload;
}

QByteArray readPacketOfType(QTcpSocket& socket, char packetType, int timeoutMs = 5000)
{
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutMs) {
        const int remainMs = qMax(1, timeoutMs - static_cast<int>(timer.elapsed()));
        const QByteArray payload = readPacket(socket, remainMs);
        if (payload.isEmpty()) {
            return {};
        }
        if (payload.at(0) == packetType) {
            return payload;
        }
    }

    return {};
}

QString md5Hex(const QByteArray& bytes)
{
    return QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex());
}

bool connectSocket(QTcpSocket& socket)
{
    socket.connectToHost(QHostAddress::LocalHost, 1234);
    return socket.waitForConnected(3000);
}

bool registerAndLogin(QTcpSocket& socket, QString* userNameOut, QString* passwordOut, qint64* userIdOut)
{
    const qint64 tel = QDateTime::currentMSecsSinceEpoch() % 100000000000LL + 13000000000LL;
    const QString userName = QStringLiteral("smoke_%1").arg(tel);
    const QString password = QStringLiteral("pw123456");

    STRU_REGISTER_RQ reg;
    qstrncpy(reg.m_szName, userName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(reg.m_szPassWord, password.toLocal8Bit().constData(), MAXSIZE);
    reg.m_tel = tel;
    if (!writePacket(socket, &reg, sizeof(reg))) {
        qCritical("register write failed");
        return false;
    }

    QByteArray payload = readPacketOfType(socket, _default_protocol_register_send);
    if (payload.size() != sizeof(STRU_REGISTER_RS) ||
        reinterpret_cast<STRU_REGISTER_RS*>(payload.data())->m_szResult != _register_success) {
        qCritical("register failed");
        return false;
    }

    STRU_LOGIN_RQ login;
    qstrncpy(login.m_szName, userName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(login.m_szPassWord, password.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &login, sizeof(login))) {
        qCritical("login write failed");
        return false;
    }

    payload = readPacketOfType(socket, _default_protocol_login_send);
    if (payload.size() != sizeof(STRU_LOGIN_RS)) {
        qCritical("login response invalid");
        return false;
    }

    auto* loginRs = reinterpret_cast<STRU_LOGIN_RS*>(payload.data());
    if (loginRs->m_szResult != _login_success || loginRs->m_userId <= 0) {
        qCritical("login failed");
        return false;
    }

    if (userNameOut) {
        *userNameOut = userName;
    }
    if (passwordOut) {
        *passwordOut = password;
    }
    if (userIdOut) {
        *userIdOut = loginRs->m_userId;
    }
    return true;
}

bool loginExisting(QTcpSocket& socket, const QString& userName, const QString& password, qint64* userIdOut)
{
    STRU_LOGIN_RQ login;
    qstrncpy(login.m_szName, userName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(login.m_szPassWord, password.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &login, sizeof(login))) {
        qCritical("secondary login write failed");
        return false;
    }

    QByteArray payload = readPacketOfType(socket, _default_protocol_login_send);
    if (payload.size() != sizeof(STRU_LOGIN_RS)) {
        qCritical("secondary login response invalid");
        return false;
    }

    auto* loginRs = reinterpret_cast<STRU_LOGIN_RS*>(payload.data());
    if (loginRs->m_szResult != _login_success || loginRs->m_userId <= 0) {
        qCritical("secondary login failed");
        return false;
    }

    if (userIdOut) {
        *userIdOut = loginRs->m_userId;
    }
    return true;
}

bool uploadSingleBlock(QTcpSocket& socket, qint64 userId, const QString& fileName, const QByteArray& content, STRU_UPLOADFILEINFO_RS* uploadInfoOut = nullptr)
{
    const QString fileMd5 = md5Hex(content);
    STRU_UPLOADFILEINFO_RQ uploadInfo;
    uploadInfo.m_userid = userId;
    uploadInfo.m_filesize = content.size();
    qstrncpy(uploadInfo.m_szFileName, fileName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(uploadInfo.m_szFileMD5, fileMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &uploadInfo, sizeof(uploadInfo))) {
        qCritical("upload info write failed");
        return false;
    }

    QByteArray payload = readPacketOfType(socket, _default_protocol_uploadfileinfo_send);
    if (payload.size() != sizeof(STRU_UPLOADFILEINFO_RS)) {
        qCritical("upload info response invalid");
        return false;
    }

    auto* uploadRs = reinterpret_cast<STRU_UPLOADFILEINFO_RS*>(payload.data());
    if (uploadInfoOut) {
        *uploadInfoOut = *uploadRs;
    }
    if (!(uploadRs->m_szResult == _fileinfo_normal ||
          uploadRs->m_szResult == _fileinfo_continue ||
          uploadRs->m_szResult == _fileinfo_speedtransfer ||
          uploadRs->m_szResult == _fileinfo_isuploaded)) {
        qCritical("upload info rejected");
        return false;
    }

    if (uploadRs->m_szResult == _fileinfo_normal || uploadRs->m_szResult == _fileinfo_continue) {
        STRU_UPLOADFILEBLOCK_RQ uploadBlock;
        uploadBlock.m_fileId = uploadRs->m_fileId;
        uploadBlock.m_fileNum = content.size();
        memcpy(uploadBlock.m_szFileContent, content.constData(), content.size());
        if (!writePacket(socket, &uploadBlock, sizeof(uploadBlock))) {
            qCritical("upload block write failed");
            return false;
        }

        payload = readPacketOfType(socket, _default_protocol_uoloadfileblock_send);
        if (payload.size() != sizeof(STRU_UPLOADFILEBLOCK_RS)) {
            qCritical("upload block response invalid");
            return false;
        }

        auto* uploadBlockRs = reinterpret_cast<STRU_UPLOADFILEBLOCK_RS*>(payload.data());
        if (uploadBlockRs->m_fileId != uploadRs->m_fileId ||
            uploadBlockRs->m_pos < content.size()) {
            qCritical("upload block ack invalid");
            return false;
        }
    }

    return true;
}
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    QTcpSocket socket;
    if (!connectSocket(socket)) {
        qCritical("connect failed");
        return 1;
    }

    QString userName;
    QString password;
    qint64 userId = 0;
    if (!registerAndLogin(socket, &userName, &password, &userId)) {
        return 2;
    }

    QByteArray content("hello netdisk smoke test");
    const QString fileMd5 = md5Hex(content);
    const QString fileName = QStringLiteral("smoke.txt");

    if (!uploadSingleBlock(socket, userId, fileName, content)) {
        return 3;
    }

    STRU_GETFILELIST_RQ listRq;
    listRq.m_userId = userId;
    if (!writePacket(socket, &listRq, sizeof(listRq))) {
        qCritical("list request write failed");
        return 4;
    }
    QByteArray payload = readPacketOfType(socket, _default_protocol_getfilelist_send);
    if (payload.size() != sizeof(STRU_GETFILELIST_RS)) {
        qCritical("list response invalid");
        return 5;
    }
    auto* listRs = reinterpret_cast<STRU_GETFILELIST_RS*>(payload.data());
    bool found = false;
    for (int i = 0; i < listRs->m_FileNum; ++i) {
        if (QString::fromLocal8Bit(listRs->m_aryInfo[i].m_szFileMD5) == fileMd5) {
            found = true;
            break;
        }
    }
    if (!found) {
        qCritical("uploaded file not found in list");
        return 6;
    }

    STRU_DOWNLOADFILEINFO_RQ downloadInfo;
    downloadInfo.m_userid = userId;
    downloadInfo.m_filesize = content.size();
    downloadInfo.m_pos = 0;
    qstrncpy(downloadInfo.m_szFileName, fileName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(downloadInfo.m_szFileMD5, fileMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &downloadInfo, sizeof(downloadInfo))) {
        qCritical("download info write failed");
        return 7;
    }
    payload = readPacketOfType(socket, _default_protocol_downloadfileinfo_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEINFO_RS)) {
        qCritical("download info response invalid");
        return 8;
    }
    auto* downloadRs = reinterpret_cast<STRU_DOWNLOADFILEINFO_RS*>(payload.data());
    if (downloadRs->m_szResult != _fileinfo_normal) {
        qCritical("download info rejected");
        return 9;
    }

    STRU_DOWNLOADFILEBLOCK_RQ downloadBlock;
    downloadBlock.m_fileId = downloadRs->m_fileId;
    downloadBlock.m_fileNum = 0;
    if (!writePacket(socket, &downloadBlock, sizeof(downloadBlock))) {
        qCritical("download block request write failed");
        return 10;
    }
    payload = readPacketOfType(socket, _default_protocol_downloadfileblock_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEBLOCK_RS)) {
        qCritical("download block response invalid");
        return 11;
    }
    auto* downBlock = reinterpret_cast<STRU_DOWNLOADFILEBLOCK_RS*>(payload.data());
    if (QByteArray(downBlock->m_szFileContent, downBlock->m_fileNum) != content) {
        qCritical("downloaded content mismatch");
        return 12;
    }

    QTcpSocket socket2;
    if (!connectSocket(socket2)) {
        qCritical("secondary connect failed");
        return 13;
    }
    qint64 secondUserId = 0;
    if (!loginExisting(socket2, userName, password, &secondUserId) || secondUserId != userId) {
        qCritical("secondary login mismatch");
        return 14;
    }

    QByteArray content2("download in parallel");
    const QString fileName2 = QStringLiteral("parallel.bin");
    if (!uploadSingleBlock(socket, userId, fileName2, content2)) {
        qCritical("parallel source upload failed");
        return 15;
    }

    const QString fileMd52 = md5Hex(content2);
    STRU_DOWNLOADFILEINFO_RQ d1;
    d1.m_userid = userId;
    d1.m_filesize = content2.size();
    d1.m_pos = 0;
    qstrncpy(d1.m_szFileName, fileName2.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(d1.m_szFileMD5, fileMd52.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &d1, sizeof(d1))) {
        qCritical("parallel download 1 request failed");
        return 16;
    }
    payload = readPacketOfType(socket, _default_protocol_downloadfileinfo_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEINFO_RS)) {
        qCritical("parallel download 1 response invalid");
        return 17;
    }
    auto* d1rs = reinterpret_cast<STRU_DOWNLOADFILEINFO_RS*>(payload.data());
    if (d1rs->m_szResult != _fileinfo_normal) {
        qCritical("parallel download 1 rejected");
        return 18;
    }

    STRU_DOWNLOADFILEINFO_RQ d2 = d1;
    if (!writePacket(socket2, &d2, sizeof(d2))) {
        qCritical("parallel download 2 request failed");
        return 19;
    }
    payload = readPacketOfType(socket2, _default_protocol_downloadfileinfo_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEINFO_RS)) {
        qCritical("parallel download 2 response invalid");
        return 20;
    }
    auto* d2rs = reinterpret_cast<STRU_DOWNLOADFILEINFO_RS*>(payload.data());
    if (d2rs->m_szResult != _fileinfo_normal) {
        qCritical("parallel download 2 rejected");
        return 21;
    }

    STRU_DOWNLOADFILEBLOCK_RQ d1block;
    d1block.m_fileId = d1rs->m_fileId;
    d1block.m_fileNum = 0;
    if (!writePacket(socket, &d1block, sizeof(d1block))) {
        qCritical("parallel download 1 block request failed");
        return 22;
    }
    payload = readPacketOfType(socket, _default_protocol_downloadfileblock_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEBLOCK_RS)) {
        qCritical("parallel download 1 block invalid");
        return 23;
    }
    auto* d1blockRs = reinterpret_cast<STRU_DOWNLOADFILEBLOCK_RS*>(payload.data());
    if (QByteArray(d1blockRs->m_szFileContent, d1blockRs->m_fileNum) != content2) {
        qCritical("parallel download 1 content mismatch");
        return 24;
    }

    STRU_DOWNLOADFILEBLOCK_RQ d2block;
    d2block.m_fileId = d2rs->m_fileId;
    d2block.m_fileNum = 0;
    if (!writePacket(socket2, &d2block, sizeof(d2block))) {
        qCritical("parallel download 2 block request failed");
        return 25;
    }
    payload = readPacketOfType(socket2, _default_protocol_downloadfileblock_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEBLOCK_RS)) {
        qCritical("parallel download 2 block invalid");
        return 26;
    }
    auto* d2blockRs = reinterpret_cast<STRU_DOWNLOADFILEBLOCK_RS*>(payload.data());
    if (QByteArray(d2blockRs->m_szFileContent, d2blockRs->m_fileNum) != content2) {
        qCritical("parallel download 2 content mismatch");
        return 27;
    }

    QTcpSocket socket3;
    if (!connectSocket(socket3)) {
        qCritical("third connect failed");
        return 28;
    }
    qint64 thirdUserId = 0;
    if (!loginExisting(socket3, userName, password, &thirdUserId) || thirdUserId != userId) {
        qCritical("third login mismatch");
        return 29;
    }

    const QByteArray busyContent("busy-upload-check");
    const QString busyName = QStringLiteral("busy.bin");
    const QString busyMd5 = md5Hex(busyContent);
    STRU_UPLOADFILEINFO_RQ busyUpload;
    busyUpload.m_userid = userId;
    busyUpload.m_filesize = busyContent.size();
    qstrncpy(busyUpload.m_szFileName, busyName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(busyUpload.m_szFileMD5, busyMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &busyUpload, sizeof(busyUpload))) {
        qCritical("busy upload 1 request failed");
        return 30;
    }
    payload = readPacketOfType(socket, _default_protocol_uploadfileinfo_send);
    if (payload.size() != sizeof(STRU_UPLOADFILEINFO_RS)) {
        qCritical("busy upload 1 response invalid");
        return 31;
    }
    auto* busyUploadRs1 = reinterpret_cast<STRU_UPLOADFILEINFO_RS*>(payload.data());
    if (!(busyUploadRs1->m_szResult == _fileinfo_normal || busyUploadRs1->m_szResult == _fileinfo_continue)) {
        qCritical("busy upload 1 did not open session");
        return 32;
    }

    if (!writePacket(socket2, &busyUpload, sizeof(busyUpload))) {
        qCritical("busy upload 2 request failed");
        return 33;
    }
    payload = readPacketOfType(socket2, _default_protocol_uploadfileinfo_send);
    if (payload.size() != sizeof(STRU_UPLOADFILEINFO_RS)) {
        qCritical("busy upload 2 response invalid");
        return 34;
    }
    auto* busyUploadRs2 = reinterpret_cast<STRU_UPLOADFILEINFO_RS*>(payload.data());
    if (busyUploadRs2->m_szResult != _fileinfo_busy) {
        qCritical("busy upload 2 should have been blocked");
        return 35;
    }

    STRU_DOWNLOADFILEINFO_RQ busyDownload;
    busyDownload.m_userid = userId;
    busyDownload.m_filesize = busyContent.size();
    busyDownload.m_pos = 0;
    qstrncpy(busyDownload.m_szFileName, busyName.toLocal8Bit().constData(), MAXSIZE);
    qstrncpy(busyDownload.m_szFileMD5, busyMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket3, &busyDownload, sizeof(busyDownload))) {
        qCritical("busy download request failed");
        return 36;
    }
    payload = readPacketOfType(socket3, _default_protocol_downloadfileinfo_send);
    if (payload.size() != sizeof(STRU_DOWNLOADFILEINFO_RS)) {
        qCritical("busy download response invalid");
        return 37;
    }
    auto* busyDownloadRs = reinterpret_cast<STRU_DOWNLOADFILEINFO_RS*>(payload.data());
    if (busyDownloadRs->m_szResult != _fileinfo_busy) {
        qCritical("busy download should have been blocked");
        return 38;
    }

    STRU_UPLOADFILEBLOCK_RQ busyUploadBlock;
    busyUploadBlock.m_fileId = busyUploadRs1->m_fileId;
    busyUploadBlock.m_fileNum = busyContent.size();
    memcpy(busyUploadBlock.m_szFileContent, busyContent.constData(), busyContent.size());
    if (!writePacket(socket, &busyUploadBlock, sizeof(busyUploadBlock))) {
        qCritical("busy upload block write failed");
        return 39;
    }
    payload = readPacketOfType(socket, _default_protocol_uoloadfileblock_send);
    if (payload.size() != sizeof(STRU_UPLOADFILEBLOCK_RS)) {
        qCritical("busy upload block response invalid");
        return 40;
    }
    auto* busyUploadBlockRs = reinterpret_cast<STRU_UPLOADFILEBLOCK_RS*>(payload.data());
    if (busyUploadBlockRs->m_pos < busyContent.size()) {
        qCritical("busy upload block ack invalid");
        return 41;
    }

    STRU_DELETEFILE_RQ deleteRq;
    deleteRq.m_userId = userId;
    qstrncpy(deleteRq.m_szFileMD5, fileMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &deleteRq, sizeof(deleteRq))) {
        qCritical("delete request write failed");
        return 42;
    }
    payload = readPacketOfType(socket, _default_protocol_deletefile_send);
    if (payload.size() != sizeof(STRU_DELETEFILE_RS) ||
        reinterpret_cast<STRU_DELETEFILE_RS*>(payload.data())->m_szResult != _delete_success) {
        qCritical("delete failed");
        return 43;
    }

    STRU_DELETEFILE_RQ deleteRq2;
    deleteRq2.m_userId = userId;
    qstrncpy(deleteRq2.m_szFileMD5, fileMd52.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &deleteRq2, sizeof(deleteRq2))) {
        qCritical("parallel delete request failed");
        return 44;
    }
    payload = readPacketOfType(socket, _default_protocol_deletefile_send);
    if (payload.size() != sizeof(STRU_DELETEFILE_RS) ||
        reinterpret_cast<STRU_DELETEFILE_RS*>(payload.data())->m_szResult != _delete_success) {
        qCritical("parallel delete failed");
        return 45;
    }

    STRU_DELETEFILE_RQ deleteRq3;
    deleteRq3.m_userId = userId;
    qstrncpy(deleteRq3.m_szFileMD5, busyMd5.toLocal8Bit().constData(), MAXSIZE);
    if (!writePacket(socket, &deleteRq3, sizeof(deleteRq3))) {
        qCritical("busy delete request failed");
        return 46;
    }
    payload = readPacketOfType(socket, _default_protocol_deletefile_send);
    if (payload.size() != sizeof(STRU_DELETEFILE_RS) ||
        reinterpret_cast<STRU_DELETEFILE_RS*>(payload.data())->m_szResult != _delete_success) {
        qCritical("busy delete failed");
        return 47;
    }

    qInfo("smoke test passed");
    return 0;
}
