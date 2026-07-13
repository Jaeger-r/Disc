#ifndef LOGIN_H
#define LOGIN_H

#include <QTabWidget>
#include <QWidget>

#include "Packdef.h"
#include "./kernel/IKernel.h"

class QLineEdit;
class QLabel;
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
    void slot_connectionStateChanged(bool connected, const QString& reason);

signals:
    void loginRequested();

private slots:
    void onRegisterClicked();
    void onLoginClicked();

private:
    void buildUi();
    void applyStyle();

    IKernel* m_pKernel = nullptr;
    QTabWidget* m_tabWidget = nullptr;
    QLabel* m_connectionLabel = nullptr;
    QLineEdit* m_registerTelEdit = nullptr;
    QLineEdit* m_registerUserEdit = nullptr;
    QLineEdit* m_registerPasswordEdit = nullptr;
    QLineEdit* m_loginUserEdit = nullptr;
    QLineEdit* m_loginPasswordEdit = nullptr;
    QPushButton* m_registerButton = nullptr;
    QPushButton* m_loginButton = nullptr;
    QString m_currentUserName;
};

#endif // LOGIN_H
