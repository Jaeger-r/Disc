#ifndef TOU_H
#define TOU_H

#endif // TOU_H

#define WINDOWSICON "./icon.png"
#define FILEICON "./file.png"

#define IP "127.0.0.1"
#define PORT 1234

#define JAEGER_DEBUG
#ifdef JAEGER_DEBUG
#define JaegerDebug() qDebug()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerInfo() qWarning()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerCritical() qCritical()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#endif
