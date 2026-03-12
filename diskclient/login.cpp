#include "login.h"
#include "ui_login.h"
#include <QDebug>
#include <stdio.h>
#include "tou.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowTitle("Disk");
    setWindowIcon(QIcon(WINDOWSICON));
}

Login::~Login()
{
    delete ui;
}

void Login::slot_register(STRU_REGISTER_RS *psrr)
{
    const char *pszResult = "register_error";
    if(psrr->m_szResult == _register_success){
        pszResult = "_register_success";
    }
    QMessageBox::information(this,"Register",pszResult);
}

//注册
void Login::on_pushButton_clicked()
{
    QString strTel = ui->lineEdit_tel->text();
    QString strUser = ui->lineEdit_user->text();
    QString strPassword = ui->lineEdit_password->text();

    STRU_REGISTER_RQ sgr;
    strcpy(sgr.m_szName,strUser.toStdString().c_str());
    strcpy(sgr.m_szPassWord,strPassword.toStdString().c_str());
    sgr.m_tel = strTel.toLongLong();

    m_pKernel->sendData((char*)&sgr,sizeof(sgr));
}
//登录
void Login::on_pushButton_2_clicked()
{
    QString strLoginUser = ui->lineEdit_loginuser->text();
    QString strLoginPassword = ui->lineEdit_login_password->text();
    strUser_ = strLoginUser;
    STRU_LOGIN_RQ slr;
    strcpy(slr.m_szName,strLoginUser.toStdString().c_str());
    strcpy(slr.m_szPassWord,strLoginPassword.toStdString().c_str());
    m_pKernel->sendData((char*)&slr,sizeof(slr));
}

