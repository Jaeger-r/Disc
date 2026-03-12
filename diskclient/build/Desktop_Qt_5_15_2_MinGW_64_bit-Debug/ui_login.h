/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label;
    QLineEdit *lineEdit_tel;
    QLineEdit *lineEdit_user;
    QLabel *label_2;
    QLineEdit *lineEdit_password;
    QLabel *label_3;
    QPushButton *pushButton;
    QWidget *widget;
    QPushButton *pushButton_2;
    QLabel *label_7;
    QLineEdit *lineEdit_loginuser;
    QLineEdit *lineEdit_login_password;
    QLabel *label_9;
    QLabel *label_11;

    void setupUi(QWidget *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName(QString::fromUtf8("Login"));
        Login->resize(800, 400);
        tabWidget = new QTabWidget(Login);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(100, 80, 600, 300));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(100, 50, 150, 40));
        QFont font;
        font.setPointSize(20);
        label->setFont(font);
        lineEdit_tel = new QLineEdit(tab);
        lineEdit_tel->setObjectName(QString::fromUtf8("lineEdit_tel"));
        lineEdit_tel->setGeometry(QRect(280, 50, 220, 40));
        lineEdit_user = new QLineEdit(tab);
        lineEdit_user->setObjectName(QString::fromUtf8("lineEdit_user"));
        lineEdit_user->setGeometry(QRect(280, 100, 220, 40));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(100, 100, 160, 40));
        label_2->setFont(font);
        lineEdit_password = new QLineEdit(tab);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setGeometry(QRect(280, 150, 220, 40));
        lineEdit_password->setEchoMode(QLineEdit::Password);
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(100, 150, 160, 40));
        label_3->setFont(font);
        pushButton = new QPushButton(tab);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(100, 210, 400, 40));
        tabWidget->addTab(tab, QString());
        widget = new QWidget();
        widget->setObjectName(QString::fromUtf8("widget"));
        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(100, 210, 400, 40));
        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(100, 50, 160, 40));
        label_7->setFont(font);
        lineEdit_loginuser = new QLineEdit(widget);
        lineEdit_loginuser->setObjectName(QString::fromUtf8("lineEdit_loginuser"));
        lineEdit_loginuser->setGeometry(QRect(280, 50, 220, 40));
        lineEdit_login_password = new QLineEdit(widget);
        lineEdit_login_password->setObjectName(QString::fromUtf8("lineEdit_login_password"));
        lineEdit_login_password->setGeometry(QRect(280, 100, 220, 40));
        lineEdit_login_password->setEchoMode(QLineEdit::Password);
        label_9 = new QLabel(widget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(100, 100, 160, 40));
        label_9->setFont(font);
        tabWidget->addTab(widget, QString());
        label_11 = new QLabel(Login);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(100, 30, 600, 40));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setStyleStrategy(QFont::PreferAntialias);
        label_11->setFont(font1);
        label_11->setAlignment(Qt::AlignCenter);

        retranslateUi(Login);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QWidget *Login)
    {
        Login->setWindowTitle(QCoreApplication::translate("Login", "Form", nullptr));
        label->setText(QCoreApplication::translate("Login", "TEL:", nullptr));
        lineEdit_tel->setPlaceholderText(QCoreApplication::translate("Login", "Please enter the tel", nullptr));
        lineEdit_user->setPlaceholderText(QCoreApplication::translate("Login", "Please enter the username", nullptr));
        label_2->setText(QCoreApplication::translate("Login", "USERNAME:", nullptr));
        lineEdit_password->setPlaceholderText(QCoreApplication::translate("Login", "Please enter the password", nullptr));
        label_3->setText(QCoreApplication::translate("Login", "PASSWORD:", nullptr));
        pushButton->setText(QCoreApplication::translate("Login", "\346\263\250\345\206\214", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("Login", "\346\263\250\345\206\214", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Login", "\347\231\273\345\275\225", nullptr));
        label_7->setText(QCoreApplication::translate("Login", "USERNAME:", nullptr));
        lineEdit_loginuser->setPlaceholderText(QCoreApplication::translate("Login", "Please enter the username", nullptr));
        lineEdit_login_password->setPlaceholderText(QCoreApplication::translate("Login", "Please enter the password", nullptr));
        label_9->setText(QCoreApplication::translate("Login", "PASSWORD:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(widget), QCoreApplication::translate("Login", "\347\231\273\345\275\225", nullptr));
        label_11->setText(QCoreApplication::translate("Login", "JAEGER_DISC", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
