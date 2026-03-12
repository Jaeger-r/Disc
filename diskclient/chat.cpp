#include "chat.h"
#include "ui_chat.h"
#include <QMessageBox>
Chat* Chat::m_chat = nullptr; // 静态指针初始化为nullptr
Chat::Chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Chat)
{
    ui->setupUi(this);
    playerName = "none";

}

Chat::~Chat()
{
    delete ui;
    delete m_pKernel;
}

void Chat::setName(QString name)
{
    playerName = name;
}
QString Chat::getName()
{
    return playerName;
}

void Chat::on_SendButton_clicked()
{
    QString UserMessage = ui->EnterBox->toPlainText().trimmed();  // 去除前后空白
    if (UserMessage.isEmpty()) {
        QMessageBox::warning(this,"发送","不能发送空消息");
        return;
    }
    QString username = getName();

    ui->EnterBox->clear();
    STRU_CHAT_RQ chat_rq;

    strcpy(chat_rq.m_userName,username.toStdString().c_str());
    strcpy(chat_rq.szbuf,UserMessage.toStdString().c_str());
}

void Chat::slot_chat(STRU_CHAT_RS *szbuf)
{
    //接收消息
    QString senderName = QString::fromUtf8(szbuf->m_userName);
    QString message = QString::fromUtf8(szbuf->szbuf);

    // 格式化显示消息的内容，发送者名称 + 消息
    QString fromattedMessage = QString("%1: %2").arg(senderName).arg(message);

    //显示到控件上
    QListWidgetItem *newItem = new QListWidgetItem(fromattedMessage);
    ui->ChatBox->addItem(newItem);

    // 自动滚动到最新消息
    ui->ChatBox->scrollToBottom();

}
