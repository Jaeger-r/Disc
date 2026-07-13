#ifndef LOGIN_H
#define LOGIN_H

#include <QTabWidget>
#include <QWidget>

#include "Packdef.h"
#include "./kernel/IKernel.h"

class QLineEdit;
class QLabel;
class QNetworkAccessManager;
class QNetworkReply;
class QPushButton;

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget* parent = nullptr);
    ~Login() override;

    void setKernel(IKernel* pKernel);
    QString currentUserName() const;
    void prepareLoginForUser(const QString& userName = QString());

public slots:
    void slot_register(STRU_REGISTER_RS* psrr);
    void slot_versioncheck(STRU_VERSION_CHECK_RS* response);
    void slot_connectionStateChanged(bool connected, const QString& reason);

signals:
    void loginRequested();

private slots:
    void onRegisterClicked();
    void onLoginClicked();
    void onUpdateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdateDownloadFinished();

private:
    void buildUi();
    void applyStyle();
    void startUpdateDownload(const QString& latestVersion, const QString& downloadUrl, bool forceUpdate);
    void setAuthControlsEnabled(bool enabled);
    void setVersionStatus(const QString& text, const QString& state);

    IKernel* m_pKernel = nullptr;
    QNetworkAccessManager* m_updateNetwork = nullptr;
    QNetworkReply* m_updateReply = nullptr;
    QTabWidget* m_tabWidget = nullptr;
    QLabel* m_connectionLabel = nullptr;
    QLabel* m_versionLabel = nullptr;
    QLabel* m_updateLabel = nullptr;
    QLineEdit* m_registerTelEdit = nullptr;
    QLineEdit* m_registerUserEdit = nullptr;
    QLineEdit* m_registerPasswordEdit = nullptr;
    QLineEdit* m_loginUserEdit = nullptr;
    QLineEdit* m_loginPasswordEdit = nullptr;
    QPushButton* m_registerButton = nullptr;
    QPushButton* m_loginButton = nullptr;
    QString m_currentUserName;
    QString m_updateVersion;
    QString m_updateDownloadPath;
    bool m_updateDownloadStarted = false;
};

#endif // LOGIN_H
