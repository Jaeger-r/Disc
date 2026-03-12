/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[20];
    char stringdata0[354];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 10), // "slot_login"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 14), // "STRU_LOGIN_RS*"
QT_MOC_LITERAL(4, 34, 16), // "slot_getfilelist"
QT_MOC_LITERAL(5, 51, 20), // "STRU_GETFILELIST_RS*"
QT_MOC_LITERAL(6, 72, 19), // "slot_uploadfileinfo"
QT_MOC_LITERAL(7, 92, 23), // "STRU_UPLOADFILEINFO_RS*"
QT_MOC_LITERAL(8, 116, 20), // "slot_uploadfileblock"
QT_MOC_LITERAL(9, 137, 24), // "STRU_UPLOADFILEBLOCK_RS*"
QT_MOC_LITERAL(10, 162, 15), // "slot_deletefile"
QT_MOC_LITERAL(11, 178, 19), // "STRU_DELETEFILE_RS*"
QT_MOC_LITERAL(12, 198, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(13, 220, 23), // "on_pushButton_2_clicked"
QT_MOC_LITERAL(14, 244, 23), // "on_pushButton_4_clicked"
QT_MOC_LITERAL(15, 268, 23), // "on_pushButton_9_clicked"
QT_MOC_LITERAL(16, 292, 26), // "on_tableWidget_cellClicked"
QT_MOC_LITERAL(17, 319, 3), // "row"
QT_MOC_LITERAL(18, 323, 6), // "column"
QT_MOC_LITERAL(19, 330, 23) // "on_pushButton_5_clicked"

    },
    "Widget\0slot_login\0\0STRU_LOGIN_RS*\0"
    "slot_getfilelist\0STRU_GETFILELIST_RS*\0"
    "slot_uploadfileinfo\0STRU_UPLOADFILEINFO_RS*\0"
    "slot_uploadfileblock\0STRU_UPLOADFILEBLOCK_RS*\0"
    "slot_deletefile\0STRU_DELETEFILE_RS*\0"
    "on_pushButton_clicked\0on_pushButton_2_clicked\0"
    "on_pushButton_4_clicked\0on_pushButton_9_clicked\0"
    "on_tableWidget_cellClicked\0row\0column\0"
    "on_pushButton_5_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x0a /* Public */,
       4,    1,   72,    2, 0x0a /* Public */,
       6,    1,   75,    2, 0x0a /* Public */,
       8,    1,   78,    2, 0x0a /* Public */,
      10,    1,   81,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    0,   85,    2, 0x08 /* Private */,
      14,    0,   86,    2, 0x08 /* Private */,
      15,    0,   87,    2, 0x08 /* Private */,
      16,    2,   88,    2, 0x08 /* Private */,
      19,    0,   93,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, 0x80000000 | 7,    2,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   17,   18,
    QMetaType::Void,

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_login((*reinterpret_cast< STRU_LOGIN_RS*(*)>(_a[1]))); break;
        case 1: _t->slot_getfilelist((*reinterpret_cast< STRU_GETFILELIST_RS*(*)>(_a[1]))); break;
        case 2: _t->slot_uploadfileinfo((*reinterpret_cast< STRU_UPLOADFILEINFO_RS*(*)>(_a[1]))); break;
        case 3: _t->slot_uploadfileblock((*reinterpret_cast< STRU_UPLOADFILEBLOCK_RS*(*)>(_a[1]))); break;
        case 4: _t->slot_deletefile((*reinterpret_cast< STRU_DELETEFILE_RS*(*)>(_a[1]))); break;
        case 5: _t->on_pushButton_clicked(); break;
        case 6: _t->on_pushButton_2_clicked(); break;
        case 7: _t->on_pushButton_4_clicked(); break;
        case 8: _t->on_pushButton_9_clicked(); break;
        case 9: _t->on_tableWidget_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->on_pushButton_5_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_Widget.data,
    qt_meta_data_Widget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
