#ifndef TOU_H
#define TOU_H

#endif // TOU_H

#define DISKPATH "D:/QT Project/DiscFileRoom/"
#define CHATLOGPATH "D:\\QT Project\\DiscServer\\chat.log"
#define DEBUG
#ifdef DEBUG
#define JaegerDebug() qDebug()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerInfo() qWarning()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#define JaegerCritical() qCritical()<< "[" << __FILE__ << ":" << __LINE__ << "]" << QDateTime::currentDateTime().toString("hh:mm:ss:zzz") << __FUNCTION__ << "()"
#endif
