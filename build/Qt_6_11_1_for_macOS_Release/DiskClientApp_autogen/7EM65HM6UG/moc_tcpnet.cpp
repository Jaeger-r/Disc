/****************************************************************************
** Meta object code from reading C++ file 'tcpnet.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../client/tcpnet.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpnet.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6TCPNetE_t {};
} // unnamed namespace

template <> constexpr inline auto TCPNet::qt_create_metaobjectdata<qt_meta_tag_ZN6TCPNetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TCPNet",
        "connectionStateChanged",
        "",
        "connected",
        "reason",
        "onReadyRead",
        "onDisconnected",
        "onBytesWritten",
        "bytes",
        "onEncrypted",
        "onSslErrors",
        "QList<QSslError>",
        "errors"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(bool, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 }, { QMetaType::QString, 4 },
        }}),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBytesWritten'
        QtMocHelpers::SlotData<void(qint64)>(7, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::LongLong, 8 },
        }}),
        // Slot 'onEncrypted'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSslErrors'
        QtMocHelpers::SlotData<void(const QList<QSslError> &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TCPNet, qt_meta_tag_ZN6TCPNetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TCPNet::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TCPNetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TCPNetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6TCPNetE_t>.metaTypes,
    nullptr
} };

void TCPNet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TCPNet *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->onReadyRead(); break;
        case 2: _t->onDisconnected(); break;
        case 3: _t->onBytesWritten((*reinterpret_cast<std::add_pointer_t<qint64>>(_a[1]))); break;
        case 4: _t->onEncrypted(); break;
        case 5: _t->onSslErrors((*reinterpret_cast<std::add_pointer_t<QList<QSslError>>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QSslError> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TCPNet::*)(bool , const QString & )>(_a, &TCPNet::connectionStateChanged, 0))
            return;
    }
}

const QMetaObject *TCPNet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TCPNet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6TCPNetE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "INet"))
        return static_cast< INet*>(this);
    return QObject::qt_metacast(_clname);
}

int TCPNet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TCPNet::connectionStateChanged(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}
QT_WARNING_POP
