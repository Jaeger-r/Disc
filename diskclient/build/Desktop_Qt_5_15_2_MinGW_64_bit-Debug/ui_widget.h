/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QTableWidget *tableWidget;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1569, 763);
        tableWidget = new QTableWidget(Widget);
        if (tableWidget->columnCount() < 4)
            tableWidget->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        if (tableWidget->rowCount() < 100)
            tableWidget->setRowCount(100);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(430, 40, 1041, 631));
        QFont font;
        font.setBold(false);
        tableWidget->setFont(font);
        tableWidget->setRowCount(100);
        tableWidget->horizontalHeader()->setVisible(true);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(true);
        tableWidget->horizontalHeader()->setDefaultSectionSize(250);
        tableWidget->verticalHeader()->setCascadingSectionResizes(true);
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(70, 40, 280, 40));
        QFont font1;
        font1.setBold(true);
        pushButton->setFont(font1);
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(70, 460, 280, 40));
        pushButton_2->setFont(font1);
        pushButton_2->setAutoRepeatInterval(100);
        pushButton_3 = new QPushButton(Widget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(70, 520, 280, 40));
        pushButton_3->setFont(font1);
        pushButton_4 = new QPushButton(Widget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(70, 580, 280, 40));
        pushButton_4->setFont(font1);
        pushButton_5 = new QPushButton(Widget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(70, 280, 280, 40));
        pushButton_5->setFont(font1);
        pushButton_6 = new QPushButton(Widget);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(70, 220, 280, 40));
        pushButton_6->setFont(font1);
        pushButton_7 = new QPushButton(Widget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(70, 340, 280, 40));
        pushButton_7->setFont(font1);
        pushButton_8 = new QPushButton(Widget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(70, 400, 280, 40));
        pushButton_8->setFont(font1);
        pushButton_9 = new QPushButton(Widget);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(70, 160, 280, 40));
        pushButton_9->setFont(font1);
        pushButton_10 = new QPushButton(Widget);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        pushButton_10->setGeometry(QRect(70, 100, 280, 40));
        pushButton_10->setFont(font1);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266\345\220\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266\345\244\247\345\260\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Widget", "\344\270\212\344\274\240\346\227\266\351\227\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266MD5", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\344\270\212\344\274\240\346\226\207\344\273\266", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "\344\270\213\350\275\275\346\226\207\344\273\266", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Widget", "\346\220\234\347\264\242\346\226\207\344\273\266", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244\346\226\207\344\273\266", nullptr));
        pushButton_5->setText(QCoreApplication::translate("Widget", "\345\210\206\344\272\253\351\223\276\346\216\245", nullptr));
        pushButton_6->setText(QCoreApplication::translate("Widget", "\346\217\220\345\217\226\351\223\276\346\216\245", nullptr));
        pushButton_7->setText(QCoreApplication::translate("Widget", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
        pushButton_8->setText(QCoreApplication::translate("Widget", "\345\210\240\351\231\244\345\245\275\345\217\213", nullptr));
        pushButton_9->setText(QCoreApplication::translate("Widget", "\350\201\212\345\244\251", nullptr));
        pushButton_10->setText(QCoreApplication::translate("Widget", "\346\220\234\347\264\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
