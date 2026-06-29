#ifndef TCPNET_H
#define TCPNET_H

#include <QObject>
#include <QByteArray>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslSocket>

#include "INet.h"

class IKernel;

class TCPNet : public QObject, public INet
{
    Q_OBJECT

public:
    explicit TCPNet(IKernel* pKernel, QObject* parent = nullptr);
    ~TCPNet() override;

    bool connectServer(const char* szip = "", quint16 sport = 0) override;
    void configureTls(bool enabled, const QString& caPath);
    void disconnectServer(const char* szerr) override;
    bool sendData(const char* szbuf, int nlen) override;
    bool isConnected() const;

signals:
    void connectionStateChanged(bool connected, const QString& reason);

private slots:
    void onReadyRead();
    void onDisconnected();
    void onBytesWritten(qint64 bytes);
    void onEncrypted();
    void onSslErrors(const QList<QSslError>& errors);

private:
    bool loadCaCertificates();
    QByteArray m_buffer;
    QByteArray m_writeBuffer;
    qint32 m_pendingPacketSize = -1;
    bool m_isShuttingDown = false;
    bool m_tlsEnabled = false;
    QString m_caPath;
    QList<QSslCertificate> m_caCertificates;
    QSslSocket m_socket;
    IKernel* m_pKernel = nullptr;
};

#endif // TCPNET_H
