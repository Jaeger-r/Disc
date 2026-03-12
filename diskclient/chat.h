#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include "packdef.h"
#include "./client/tcpnet.h"
#include "./kernel/IKernel.h"

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT
private:
    explicit Chat(QWidget *parent = nullptr);
public:
    ~Chat();
public:
    static Chat* getChat()
    {
        if (m_chat == nullptr)
        {
            m_chat = new Chat();
        }
        return m_chat;
    }
    void setCore(IKernel*pKernel) {
        m_pKernel = pKernel;
    }
    void setName(QString name);
    QString getName();
public slots:
    void slot_chat(STRU_CHAT_RS*szbuf);
private slots:
    void on_SendButton_clicked();

private:
    Ui::Chat *ui;
    IKernel *m_pKernel;
    QString playerName;
    static Chat *m_chat;
};

#endif // CHAT_H
