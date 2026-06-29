/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../widget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6WidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto Widget::qt_create_metaobjectdata<qt_meta_tag_ZN6WidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Widget",
        "slot_login",
        "",
        "STRU_LOGIN_RS*",
        "slot_getfilelist",
        "STRU_GETFILELIST_RS*",
        "slot_uploadfileinfo",
        "STRU_UPLOADFILEINFO_RS*",
        "slot_uploadfileblock",
        "STRU_UPLOADFILEBLOCK_RS*",
        "slot_deletefile",
        "STRU_DELETEFILE_RS*",
        "slot_renamefile",
        "STRU_RENAMEFILE_RS*",
        "slot_filesync",
        "STRU_FILESYNC_RS*",
        "slot_downloadfileinfo",
        "STRU_DOWNLOADFILEINFO_RS*",
        "slot_downloadfileblock",
        "STRU_DOWNLOADFILEBLOCK_RS*",
        "slot_chat",
        "STRU_CHAT_RS*",
        "slot_onlineusers",
        "STRU_ONLINE_USERS_RS*",
        "slot_privatechat",
        "STRU_PRIVATE_CHAT_RS*",
        "slot_privatehistory",
        "STRU_PRIVATE_HISTORY_RS*",
        "slot_transfercontrol",
        "STRU_TRANSFERCONTROL_RS*",
        "slot_connectionStateChanged",
        "connected",
        "reason",
        "onUploadClicked",
        "onDownloadClicked",
        "onDeleteClicked",
        "onChatClicked",
        "onRefreshClicked",
        "onShareClicked",
        "onSelectionChanged",
        "onFileTableContextMenuRequested",
        "QPoint",
        "pos",
        "onTransfersClicked",
        "onTransferPauseRequested",
        "taskKey",
        "onTransferResumeRequested",
        "onTransferCancelRequested",
        "processUploadPump",
        "updateTransferRates"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'slot_login'
        QtMocHelpers::SlotData<void(STRU_LOGIN_RS *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Slot 'slot_getfilelist'
        QtMocHelpers::SlotData<void(STRU_GETFILELIST_RS *)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 2 },
        }}),
        // Slot 'slot_uploadfileinfo'
        QtMocHelpers::SlotData<void(STRU_UPLOADFILEINFO_RS *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 2 },
        }}),
        // Slot 'slot_uploadfileblock'
        QtMocHelpers::SlotData<void(STRU_UPLOADFILEBLOCK_RS *)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Slot 'slot_deletefile'
        QtMocHelpers::SlotData<void(STRU_DELETEFILE_RS *)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 2 },
        }}),
        // Slot 'slot_renamefile'
        QtMocHelpers::SlotData<void(STRU_RENAMEFILE_RS *)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 2 },
        }}),
        // Slot 'slot_filesync'
        QtMocHelpers::SlotData<void(STRU_FILESYNC_RS *)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 2 },
        }}),
        // Slot 'slot_downloadfileinfo'
        QtMocHelpers::SlotData<void(STRU_DOWNLOADFILEINFO_RS *)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 2 },
        }}),
        // Slot 'slot_downloadfileblock'
        QtMocHelpers::SlotData<void(STRU_DOWNLOADFILEBLOCK_RS *)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 2 },
        }}),
        // Slot 'slot_chat'
        QtMocHelpers::SlotData<void(STRU_CHAT_RS *)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 21, 2 },
        }}),
        // Slot 'slot_onlineusers'
        QtMocHelpers::SlotData<void(STRU_ONLINE_USERS_RS *)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 23, 2 },
        }}),
        // Slot 'slot_privatechat'
        QtMocHelpers::SlotData<void(STRU_PRIVATE_CHAT_RS *)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 25, 2 },
        }}),
        // Slot 'slot_privatehistory'
        QtMocHelpers::SlotData<void(STRU_PRIVATE_HISTORY_RS *)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 27, 2 },
        }}),
        // Slot 'slot_transfercontrol'
        QtMocHelpers::SlotData<void(STRU_TRANSFERCONTROL_RS *)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 29, 2 },
        }}),
        // Slot 'slot_connectionStateChanged'
        QtMocHelpers::SlotData<void(bool, const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 31 }, { QMetaType::QString, 32 },
        }}),
        // Slot 'onUploadClicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDownloadClicked'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeleteClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChatClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onShareClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelectionChanged'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onFileTableContextMenuRequested'
        QtMocHelpers::SlotData<void(const QPoint &)>(40, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 41, 42 },
        }}),
        // Slot 'onTransfersClicked'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTransferPauseRequested'
        QtMocHelpers::SlotData<void(const QString &)>(44, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 45 },
        }}),
        // Slot 'onTransferResumeRequested'
        QtMocHelpers::SlotData<void(const QString &)>(46, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 45 },
        }}),
        // Slot 'onTransferCancelRequested'
        QtMocHelpers::SlotData<void(const QString &)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 45 },
        }}),
        // Slot 'processUploadPump'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'updateTransferRates'
        QtMocHelpers::SlotData<void()>(49, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Widget, qt_meta_tag_ZN6WidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6WidgetE_t>.metaTypes,
    nullptr
} };

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Widget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->slot_login((*reinterpret_cast<std::add_pointer_t<STRU_LOGIN_RS*>>(_a[1]))); break;
        case 1: _t->slot_getfilelist((*reinterpret_cast<std::add_pointer_t<STRU_GETFILELIST_RS*>>(_a[1]))); break;
        case 2: _t->slot_uploadfileinfo((*reinterpret_cast<std::add_pointer_t<STRU_UPLOADFILEINFO_RS*>>(_a[1]))); break;
        case 3: _t->slot_uploadfileblock((*reinterpret_cast<std::add_pointer_t<STRU_UPLOADFILEBLOCK_RS*>>(_a[1]))); break;
        case 4: _t->slot_deletefile((*reinterpret_cast<std::add_pointer_t<STRU_DELETEFILE_RS*>>(_a[1]))); break;
        case 5: _t->slot_renamefile((*reinterpret_cast<std::add_pointer_t<STRU_RENAMEFILE_RS*>>(_a[1]))); break;
        case 6: _t->slot_filesync((*reinterpret_cast<std::add_pointer_t<STRU_FILESYNC_RS*>>(_a[1]))); break;
        case 7: _t->slot_downloadfileinfo((*reinterpret_cast<std::add_pointer_t<STRU_DOWNLOADFILEINFO_RS*>>(_a[1]))); break;
        case 8: _t->slot_downloadfileblock((*reinterpret_cast<std::add_pointer_t<STRU_DOWNLOADFILEBLOCK_RS*>>(_a[1]))); break;
        case 9: _t->slot_chat((*reinterpret_cast<std::add_pointer_t<STRU_CHAT_RS*>>(_a[1]))); break;
        case 10: _t->slot_onlineusers((*reinterpret_cast<std::add_pointer_t<STRU_ONLINE_USERS_RS*>>(_a[1]))); break;
        case 11: _t->slot_privatechat((*reinterpret_cast<std::add_pointer_t<STRU_PRIVATE_CHAT_RS*>>(_a[1]))); break;
        case 12: _t->slot_privatehistory((*reinterpret_cast<std::add_pointer_t<STRU_PRIVATE_HISTORY_RS*>>(_a[1]))); break;
        case 13: _t->slot_transfercontrol((*reinterpret_cast<std::add_pointer_t<STRU_TRANSFERCONTROL_RS*>>(_a[1]))); break;
        case 14: _t->slot_connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: _t->onUploadClicked(); break;
        case 16: _t->onDownloadClicked(); break;
        case 17: _t->onDeleteClicked(); break;
        case 18: _t->onChatClicked(); break;
        case 19: _t->onRefreshClicked(); break;
        case 20: _t->onShareClicked(); break;
        case 21: _t->onSelectionChanged(); break;
        case 22: _t->onFileTableContextMenuRequested((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 23: _t->onTransfersClicked(); break;
        case 24: _t->onTransferPauseRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 25: _t->onTransferResumeRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 26: _t->onTransferCancelRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->processUploadPump(); break;
        case 28: _t->updateTransferRates(); break;
        default: ;
        }
    }
}

const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
