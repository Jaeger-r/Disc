/****************************************************************************
** Meta object code from reading C++ file 'tcpkernel.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../kernel/tcpkernel.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpkernel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN9TCPKernelE_t {};
} // unnamed namespace

template <> constexpr inline auto TCPKernel::qt_create_metaobjectdata<qt_meta_tag_ZN9TCPKernelE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TCPKernel",
        "signal_register",
        "",
        "STRU_REGISTER_RS*",
        "signal_login",
        "STRU_LOGIN_RS*",
        "signal_getfilelist",
        "STRU_GETFILELIST_RS*",
        "signal_uploadfileinfo",
        "STRU_UPLOADFILEINFO_RS*",
        "signal_uploadfileblock",
        "STRU_UPLOADFILEBLOCK_RS*",
        "signal_deletefileinfo",
        "STRU_DELETEFILE_RS*",
        "signal_renamefileinfo",
        "STRU_RENAMEFILE_RS*",
        "signal_filesync",
        "STRU_FILESYNC_RS*",
        "signal_downloadfileinfo",
        "STRU_DOWNLOADFILEINFO_RS*",
        "signal_downloadfileblock",
        "STRU_DOWNLOADFILEBLOCK_RS*",
        "signal_chat",
        "STRU_CHAT_RS*",
        "signal_transfercontrol",
        "STRU_TRANSFERCONTROL_RS*",
        "signal_connectionStateChanged",
        "connected",
        "reason"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'signal_register'
        QtMocHelpers::SignalData<void(STRU_REGISTER_RS *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'signal_login'
        QtMocHelpers::SignalData<void(STRU_LOGIN_RS *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Signal 'signal_getfilelist'
        QtMocHelpers::SignalData<void(STRU_GETFILELIST_RS *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 2 },
        }}),
        // Signal 'signal_uploadfileinfo'
        QtMocHelpers::SignalData<void(STRU_UPLOADFILEINFO_RS *)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Signal 'signal_uploadfileblock'
        QtMocHelpers::SignalData<void(STRU_UPLOADFILEBLOCK_RS *)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 2 },
        }}),
        // Signal 'signal_deletefileinfo'
        QtMocHelpers::SignalData<void(STRU_DELETEFILE_RS *)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 2 },
        }}),
        // Signal 'signal_renamefileinfo'
        QtMocHelpers::SignalData<void(STRU_RENAMEFILE_RS *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 2 },
        }}),
        // Signal 'signal_filesync'
        QtMocHelpers::SignalData<void(STRU_FILESYNC_RS *)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 2 },
        }}),
        // Signal 'signal_downloadfileinfo'
        QtMocHelpers::SignalData<void(STRU_DOWNLOADFILEINFO_RS *)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 2 },
        }}),
        // Signal 'signal_downloadfileblock'
        QtMocHelpers::SignalData<void(STRU_DOWNLOADFILEBLOCK_RS *)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 21, 2 },
        }}),
        // Signal 'signal_chat'
        QtMocHelpers::SignalData<void(STRU_CHAT_RS *)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 2 },
        }}),
        // Signal 'signal_transfercontrol'
        QtMocHelpers::SignalData<void(STRU_TRANSFERCONTROL_RS *)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 25, 2 },
        }}),
        // Signal 'signal_connectionStateChanged'
        QtMocHelpers::SignalData<void(bool, const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 27 }, { QMetaType::QString, 28 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TCPKernel, qt_meta_tag_ZN9TCPKernelE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TCPKernel::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPKernelE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPKernelE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9TCPKernelE_t>.metaTypes,
    nullptr
} };

void TCPKernel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TCPKernel *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->signal_register((*reinterpret_cast<std::add_pointer_t<STRU_REGISTER_RS*>>(_a[1]))); break;
        case 1: _t->signal_login((*reinterpret_cast<std::add_pointer_t<STRU_LOGIN_RS*>>(_a[1]))); break;
        case 2: _t->signal_getfilelist((*reinterpret_cast<std::add_pointer_t<STRU_GETFILELIST_RS*>>(_a[1]))); break;
        case 3: _t->signal_uploadfileinfo((*reinterpret_cast<std::add_pointer_t<STRU_UPLOADFILEINFO_RS*>>(_a[1]))); break;
        case 4: _t->signal_uploadfileblock((*reinterpret_cast<std::add_pointer_t<STRU_UPLOADFILEBLOCK_RS*>>(_a[1]))); break;
        case 5: _t->signal_deletefileinfo((*reinterpret_cast<std::add_pointer_t<STRU_DELETEFILE_RS*>>(_a[1]))); break;
        case 6: _t->signal_renamefileinfo((*reinterpret_cast<std::add_pointer_t<STRU_RENAMEFILE_RS*>>(_a[1]))); break;
        case 7: _t->signal_filesync((*reinterpret_cast<std::add_pointer_t<STRU_FILESYNC_RS*>>(_a[1]))); break;
        case 8: _t->signal_downloadfileinfo((*reinterpret_cast<std::add_pointer_t<STRU_DOWNLOADFILEINFO_RS*>>(_a[1]))); break;
        case 9: _t->signal_downloadfileblock((*reinterpret_cast<std::add_pointer_t<STRU_DOWNLOADFILEBLOCK_RS*>>(_a[1]))); break;
        case 10: _t->signal_chat((*reinterpret_cast<std::add_pointer_t<STRU_CHAT_RS*>>(_a[1]))); break;
        case 11: _t->signal_transfercontrol((*reinterpret_cast<std::add_pointer_t<STRU_TRANSFERCONTROL_RS*>>(_a[1]))); break;
        case 12: _t->signal_connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_REGISTER_RS * )>(_a, &TCPKernel::signal_register, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_LOGIN_RS * )>(_a, &TCPKernel::signal_login, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_GETFILELIST_RS * )>(_a, &TCPKernel::signal_getfilelist, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_UPLOADFILEINFO_RS * )>(_a, &TCPKernel::signal_uploadfileinfo, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_UPLOADFILEBLOCK_RS * )>(_a, &TCPKernel::signal_uploadfileblock, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_DELETEFILE_RS * )>(_a, &TCPKernel::signal_deletefileinfo, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_RENAMEFILE_RS * )>(_a, &TCPKernel::signal_renamefileinfo, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_FILESYNC_RS * )>(_a, &TCPKernel::signal_filesync, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_DOWNLOADFILEINFO_RS * )>(_a, &TCPKernel::signal_downloadfileinfo, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_DOWNLOADFILEBLOCK_RS * )>(_a, &TCPKernel::signal_downloadfileblock, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_CHAT_RS * )>(_a, &TCPKernel::signal_chat, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(STRU_TRANSFERCONTROL_RS * )>(_a, &TCPKernel::signal_transfercontrol, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (TCPKernel::*)(bool , const QString & )>(_a, &TCPKernel::signal_connectionStateChanged, 12))
            return;
    }
}

const QMetaObject *TCPKernel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TCPKernel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TCPKernelE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "IKernel"))
        return static_cast< IKernel*>(this);
    return QObject::qt_metacast(_clname);
}

int TCPKernel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void TCPKernel::signal_register(STRU_REGISTER_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void TCPKernel::signal_login(STRU_LOGIN_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void TCPKernel::signal_getfilelist(STRU_GETFILELIST_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void TCPKernel::signal_uploadfileinfo(STRU_UPLOADFILEINFO_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void TCPKernel::signal_uploadfileblock(STRU_UPLOADFILEBLOCK_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void TCPKernel::signal_deletefileinfo(STRU_DELETEFILE_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void TCPKernel::signal_renamefileinfo(STRU_RENAMEFILE_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void TCPKernel::signal_filesync(STRU_FILESYNC_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void TCPKernel::signal_downloadfileinfo(STRU_DOWNLOADFILEINFO_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void TCPKernel::signal_downloadfileblock(STRU_DOWNLOADFILEBLOCK_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void TCPKernel::signal_chat(STRU_CHAT_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void TCPKernel::signal_transfercontrol(STRU_TRANSFERCONTROL_RS * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void TCPKernel::signal_connectionStateChanged(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2);
}
QT_WARNING_POP
