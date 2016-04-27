/****************************************************************************
** Meta object code from reading C++ file 'chart_thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "chart_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chart_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ChartThread_t {
    QByteArrayData data[10];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChartThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChartThread_t qt_meta_stringdata_ChartThread = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ChartThread"
QT_MOC_LITERAL(1, 12, 19), // "tempTagsFoundSignal"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 16), // "QList<SensorTag>"
QT_MOC_LITERAL(4, 50, 22), // "tempTagsMeasuredSignal"
QT_MOC_LITERAL(5, 73, 16), // "QList<QDateTime>"
QT_MOC_LITERAL(6, 90, 20), // "moistTagsFoundSignal"
QT_MOC_LITERAL(7, 111, 23), // "moistTagsMeasuredSignal"
QT_MOC_LITERAL(8, 135, 22), // "tempCodeMeasuredSignal"
QT_MOC_LITERAL(9, 158, 4) // "code"

    },
    "ChartThread\0tempTagsFoundSignal\0\0"
    "QList<SensorTag>\0tempTagsMeasuredSignal\0"
    "QList<QDateTime>\0moistTagsFoundSignal\0"
    "moistTagsMeasuredSignal\0tempCodeMeasuredSignal\0"
    "code"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChartThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    2,   42,    2, 0x06 /* Public */,
       6,    1,   47,    2, 0x06 /* Public */,
       7,    2,   50,    2, 0x06 /* Public */,
       8,    1,   55,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    2,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 5,    2,    2,
    QMetaType::Void, QMetaType::Float,    9,

       0        // eod
};

void ChartThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ChartThread *_t = static_cast<ChartThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tempTagsFoundSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 1: _t->tempTagsMeasuredSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1])),(*reinterpret_cast< QList<QDateTime>(*)>(_a[2]))); break;
        case 2: _t->moistTagsFoundSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 3: _t->moistTagsMeasuredSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1])),(*reinterpret_cast< QList<QDateTime>(*)>(_a[2]))); break;
        case 4: _t->tempCodeMeasuredSignal((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QDateTime> >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QDateTime> >(); break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ChartThread::*_t)(QList<SensorTag> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChartThread::tempTagsFoundSignal)) {
                *result = 0;
            }
        }
        {
            typedef void (ChartThread::*_t)(QList<SensorTag> , QList<QDateTime> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChartThread::tempTagsMeasuredSignal)) {
                *result = 1;
            }
        }
        {
            typedef void (ChartThread::*_t)(QList<SensorTag> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChartThread::moistTagsFoundSignal)) {
                *result = 2;
            }
        }
        {
            typedef void (ChartThread::*_t)(QList<SensorTag> , QList<QDateTime> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChartThread::moistTagsMeasuredSignal)) {
                *result = 3;
            }
        }
        {
            typedef void (ChartThread::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ChartThread::tempCodeMeasuredSignal)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject ChartThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ChartThread.data,
      qt_meta_data_ChartThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ChartThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChartThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ChartThread.stringdata0))
        return static_cast<void*>(const_cast< ChartThread*>(this));
    return QThread::qt_metacast(_clname);
}

int ChartThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ChartThread::tempTagsFoundSignal(QList<SensorTag> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChartThread::tempTagsMeasuredSignal(QList<SensorTag> _t1, QList<QDateTime> _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ChartThread::moistTagsFoundSignal(QList<SensorTag> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ChartThread::moistTagsMeasuredSignal(QList<SensorTag> _t1, QList<QDateTime> _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ChartThread::tempCodeMeasuredSignal(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
