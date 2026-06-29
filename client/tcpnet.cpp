#include "tcpnet.h"

#include <QFile>
#include <QHostAddress>

#include "../kernel/IKernel.h"

TCPNet::TCPNet(IKernel* pKernel, QObject* parent)
    : QObject(parent)
    , m_pKernel(pKernel)
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &TCPNet::onReadyRead);
    connect(&m_socket, &QTcpSocket::disconnected, this, &TCPNet::onDisconnected);
    connect(&m_socket, &QTcpSocket::bytesWritten, this, &TCPNet::onBytesWritten);
    connect(&m_socket, &QSslSocket::encrypted, this, &TCPNet::onEncrypted);
    connect(&m_socket, &QSslSocket::sslErrors, this, &TCPNet::onSslErrors);
}

TCPNet::~TCPNet()
{
    m_isShuttingDown = true;
    m_socket.blockSignals(true);
    disconnectServer("client destroyed");
}

bool TCPNet::connectServer(const char* szip, quint16 sport)
{
    const QString host = QString::fromLocal8Bit(szip);
    if (m_tlsEnabled) {
        if (!loadCaCertificates()) {
            emit connectionStateChanged(false, QStringLiteral("TLS CA 证书加载失败"));
            return false;
        }
        QSslConfiguration configuration = m_socket.sslConfiguration();
        configuration.setPeerVerifyMode(QSslSocket::VerifyPeer);
        configuration.setProtocol(QSsl::TlsV1_2OrLater);
        configuration.setCaCertificates(m_caCertificates);
        m_socket.setSslConfiguration(configuration);
        m_socket.connectToHostEncrypted(host, sport);
        const bool ok = m_socket.waitForEncrypted(5000);
        emit connectionStateChanged(ok, ok ? QStringLiteral("TLS connected") : m_socket.errorString());
        return ok;
    }

    m_socket.connectToHost(host, sport);
    const bool ok = m_socket.waitForConnected(3000);
    emit connectionStateChanged(ok, ok ? QStringLiteral("connected") : m_socket.errorString());
    return ok;
}

void TCPNet::configureTls(bool enabled, const QString& caPath)
{
    m_tlsEnabled = enabled;
    m_caPath = caPath;
}

void TCPNet::disconnectServer(const char* szerr)
{
    Q_UNUSED(szerr);
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        m_socket.disconnectFromHost();
        if (m_socket.state() != QAbstractSocket::UnconnectedState) {
            m_socket.waitForDisconnected(1000);
        }
    }
    m_buffer.clear();
    m_writeBuffer.clear();
    m_pendingPacketSize = -1;
}

bool TCPNet::sendData(const char* szbuf, int nlen)
{
    if (!szbuf || nlen <= 0 || m_socket.state() != QAbstractSocket::ConnectedState) {
        return false;
    }

    qint32 packetSize = nlen;
    m_writeBuffer.append(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));
    m_writeBuffer.append(szbuf, nlen);

    const qint64 written = m_socket.write(m_writeBuffer);
    if (written < 0) {
        return false;
    }
    if (written > 0) {
        m_writeBuffer.remove(0, static_cast<int>(written));
    }
    return true;
}

bool TCPNet::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void TCPNet::onReadyRead()
{
    m_buffer.append(m_socket.readAll());

    while (true) {
        if (m_pendingPacketSize < 0) {
            if (m_buffer.size() < static_cast<int>(sizeof(qint32))) {
                return;
            }
            memcpy(&m_pendingPacketSize, m_buffer.constData(), sizeof(qint32));
            m_buffer.remove(0, sizeof(qint32));
            if (m_pendingPacketSize <= 0) {
                m_pendingPacketSize = -1;
                continue;
            }
        }

        if (m_buffer.size() < m_pendingPacketSize) {
            return;
        }

        QByteArray payload = m_buffer.left(m_pendingPacketSize);
        m_buffer.remove(0, m_pendingPacketSize);
        m_pendingPacketSize = -1;

        if (m_pKernel) {
            m_pKernel->dealData(payload.constData());
        }
    }
}

void TCPNet::onDisconnected()
{
    m_buffer.clear();
    m_writeBuffer.clear();
    m_pendingPacketSize = -1;
    if (m_isShuttingDown) {
        return;
    }
    emit connectionStateChanged(false, m_socket.errorString().isEmpty() ? QStringLiteral("disconnected") : m_socket.errorString());
}

void TCPNet::onBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    if (m_writeBuffer.isEmpty()) {
        return;
    }

    const qint64 written = m_socket.write(m_writeBuffer);
    if (written > 0) {
        m_writeBuffer.remove(0, static_cast<int>(written));
    }
}

void TCPNet::onEncrypted()
{
    emit connectionStateChanged(true, QStringLiteral("TLS encrypted"));
}

void TCPNet::onSslErrors(const QList<QSslError>& errors)
{
    QStringList messages;
    for (const QSslError& error : errors) {
        messages.push_back(error.errorString());
    }
    emit connectionStateChanged(false, messages.join(QStringLiteral("; ")));
}

bool TCPNet::loadCaCertificates()
{
    QFile caFile(m_caPath);
    if (!caFile.open(QIODevice::ReadOnly)) {
        return false;
    }
    const QByteArray caBytes = caFile.readAll();
    caFile.close();

    m_caCertificates = QSslCertificate::fromData(caBytes, QSsl::Pem);
    return !m_caCertificates.isEmpty();
}
