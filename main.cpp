#include "widget.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString appDir = QCoreApplication::applicationDirPath();
    a.setWindowIcon(QIcon(QStringLiteral(":/app/DiskClientIcon.png")));
    const QString configPath =
        QDir(appDir).filePath(QStringLiteral("diskclient.ini"));
    if (!QFileInfo::exists(configPath)) {
        QSettings settings(configPath, QSettings::IniFormat);
        settings.setValue(QStringLiteral("network/serverHost"), QStringLiteral("43.142.3.188"));
        settings.setValue(QStringLiteral("network/serverPort"), 1234);
        settings.setValue(QStringLiteral("network/tlsEnabled"), true);
        settings.setValue(QStringLiteral("network/tlsCaPath"),
                          QDir(appDir).filePath(QStringLiteral("tls/ca.crt")));
        settings.sync();
    }
    Widget w;
    return a.exec();
}
