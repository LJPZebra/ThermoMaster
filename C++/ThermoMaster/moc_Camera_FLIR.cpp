/****************************************************************************
** Meta object code from reading C++ file 'Camera_FLIR.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Camera_FLIR.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Camera_FLIR.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LowLevel_FLIR_t {
    QByteArrayData data[6];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LowLevel_FLIR_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LowLevel_FLIR_t qt_meta_stringdata_LowLevel_FLIR = {
    {
QT_MOC_LITERAL(0, 0, 13), // "LowLevel_FLIR"
QT_MOC_LITERAL(1, 14, 8), // "newImage"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "Image_FLIR"
QT_MOC_LITERAL(4, 35, 12), // "display_info"
QT_MOC_LITERAL(5, 48, 4) // "grab"

    },
    "LowLevel_FLIR\0newImage\0\0Image_FLIR\0"
    "display_info\0grab"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LowLevel_FLIR[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   32,    2, 0x0a /* Public */,
       5,    0,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void LowLevel_FLIR::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LowLevel_FLIR *_t = static_cast<LowLevel_FLIR *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newImage((*reinterpret_cast< Image_FLIR(*)>(_a[1]))); break;
        case 1: _t->display_info(); break;
        case 2: _t->grab(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Image_FLIR >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (LowLevel_FLIR::*_t)(Image_FLIR );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&LowLevel_FLIR::newImage)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject LowLevel_FLIR::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LowLevel_FLIR.data,
      qt_meta_data_LowLevel_FLIR,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *LowLevel_FLIR::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LowLevel_FLIR::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LowLevel_FLIR.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LowLevel_FLIR::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LowLevel_FLIR::newImage(Image_FLIR _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_Camera_FLIR_t {
    QByteArrayData data[7];
    char stringdata0[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Camera_FLIR_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Camera_FLIR_t qt_meta_stringdata_Camera_FLIR = {
    {
QT_MOC_LITERAL(0, 0, 11), // "Camera_FLIR"
QT_MOC_LITERAL(1, 12, 18), // "newImageForDisplay"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 12), // "display_info"
QT_MOC_LITERAL(4, 45, 8), // "newImage"
QT_MOC_LITERAL(5, 54, 10), // "Image_FLIR"
QT_MOC_LITERAL(6, 65, 10) // "stopCamera"

    },
    "Camera_FLIR\0newImageForDisplay\0\0"
    "display_info\0newImage\0Image_FLIR\0"
    "stopCamera"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Camera_FLIR[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   37,    2, 0x0a /* Public */,
       4,    1,   38,    2, 0x0a /* Public */,
       6,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPixmap,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void,

       0        // eod
};

void Camera_FLIR::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Camera_FLIR *_t = static_cast<Camera_FLIR *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newImageForDisplay((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 1: _t->display_info(); break;
        case 2: _t->newImage((*reinterpret_cast< Image_FLIR(*)>(_a[1]))); break;
        case 3: _t->stopCamera(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Image_FLIR >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Camera_FLIR::*_t)(QPixmap );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Camera_FLIR::newImageForDisplay)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject Camera_FLIR::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Camera_FLIR.data,
      qt_meta_data_Camera_FLIR,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Camera_FLIR::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Camera_FLIR::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Camera_FLIR.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Camera_FLIR::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Camera_FLIR::newImageForDisplay(QPixmap _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
