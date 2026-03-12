#include <QCoreApplication>
#include "./kernel/TCPKernel.h"
#include <iostream>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QDir>
#include <QFileInfo>
#include "tou.h"
static QMutex logMutex;

static QString colorByLevel(const QString &level)
{
    if (level == "ERROR" || level == "FATAL")
        return "\033[31m";   // 红
    if (level == "WARN")
        return "\033[33m";   // 黄
    if (level == "INFO")
        return "\033[32m";   // 绿
    if (level == "DEBUG")
        return "\033[36m";   // 青
    return "\033[0m";
}

void messageHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &msg)
{
    QMutexLocker locker(&logMutex);

    QString logFilePath =
        QCoreApplication::applicationDirPath()
        + "/logs/server/server.log";

    QFileInfo fileInfo(logFilePath);
    QDir().mkpath(fileInfo.absolutePath());

    QFile file(logFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);

    QString level;
    switch (type) {
    case QtDebugMsg:    level = "DEBUG"; break;
    case QtInfoMsg:     level = "INFO";  break;
    case QtWarningMsg:  level = "WARN";  break;
    case QtCriticalMsg: level = "ERROR"; break;
    case QtFatalMsg:    level = "FATAL"; break;
    }

    QString text =
        QString("[%1] [%2] [%3:%4] %5()\n    %6")
            .arg(QDateTime::currentDateTime()
                     .toString("yyyy-MM-dd hh:mm:ss.zzz"))
            .arg(level)
            .arg(context.file ? context.file : "unknown")
            .arg(context.line)
            .arg(context.function ? context.function : "unknown")
            .arg(msg);

    // 1️⃣ 写文件（无颜色）
    out << text << "\n";
    out.flush();

    // 2️⃣ 控制台输出（带颜色）
    QString color = colorByLevel(level);
    fprintf(stderr, "%s%s\033[0m\n",
            color.toLocal8Bit().constData(),
            text.toLocal8Bit().constData());

    if (type == QtFatalMsg)
        abort();
}
int main(int argc, char *argv[])
{
#ifdef DEBUG
    qInstallMessageHandler(messageHandler);
#endif
    QCoreApplication a(argc, argv);
    IKernel *p = TCPKernel::getKernel();
    if(p->open()){
        cout<<"Server is running"<<endl;
    }else{
        cout<<"Server failed"<<endl;
    }

    return a.exec();
}
