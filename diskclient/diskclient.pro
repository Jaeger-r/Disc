QT       += core gui
CONFIG += console
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chat.cpp \
    login.cpp \
    main.cpp \
    md5.cpp \
    widget.cpp

HEADERS += \
    Packdef.h \
    chat.h \
    login.h \
    md5.h \
    tou.h \
    widget.h

FORMS += \
    chat.ui \
    login.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include (./client/client.pri)
include (./kernel/kernel.pri)

RESOURCES +=
