#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "Packdef.h"
#include "./kernel/IKernel.h"
#include <QMessageBox>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
public slots:
    void slot_register(STRU_REGISTER_RS * psrr);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

public:
    void setKernel(IKernel*pKernel){
        m_pKernel = pKernel;
    }
    QString GetUserName(){
        return strUser_;
    }
private:
    QString strUser_;
    Ui::Login *ui;
    IKernel*m_pKernel;
};

#endif // LOGIN_H
