/****************************************************************************
** Meta object code from reading C++ file 'kit_model.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "kit_model.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'kit_model.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_KitModel_t {
    QByteArrayData data[10];
    char stringdata[182];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_KitModel_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_KitModel_t qt_meta_stringdata_KitModel = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 20),
QT_MOC_LITERAL(2, 30, 0),
QT_MOC_LITERAL(3, 31, 16),
QT_MOC_LITERAL(4, 48, 29),
QT_MOC_LITERAL(5, 78, 21),
QT_MOC_LITERAL(6, 100, 30),
QT_MOC_LITERAL(7, 131, 19),
QT_MOC_LITERAL(8, 151, 17),
QT_MOC_LITERAL(9, 169, 12)
    },
    "KitModel\0updateTempTagsSignal\0\0"
    "QList<SensorTag>\0updateTempTagSelectionsSignal\0"
    "updateMoistTagsSignal\0"
    "updateMoistTagSelectionsSignal\0"
    "antennaTuningSignal\0bandChangedSignal\0"
    "FreqBandEnum"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_KitModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,
       6,    0,   51,    2, 0x06 /* Public */,
       7,    2,   52,    2, 0x06 /* Public */,
       8,    1,   57,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, 0x80000000 | 9,    2,

       0        // eod
};

void KitModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        KitModel *_t = static_cast<KitModel *>(_o);
        switch (_id) {
        case 0: _t->updateTempTagsSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 1: _t->updateTempTagSelectionsSignal(); break;
        case 2: _t->updateMoistTagsSignal((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 3: _t->updateMoistTagSelectionsSignal(); break;
        case 4: _t->antennaTuningSignal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->bandChangedSignal((*reinterpret_cast< FreqBandEnum(*)>(_a[1]))); break;
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
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (KitModel::*_t)(QList<SensorTag> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::updateTempTagsSignal)) {
                *result = 0;
            }
        }
        {
            typedef void (KitModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::updateTempTagSelectionsSignal)) {
                *result = 1;
            }
        }
        {
            typedef void (KitModel::*_t)(QList<SensorTag> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::updateMoistTagsSignal)) {
                *result = 2;
            }
        }
        {
            typedef void (KitModel::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::updateMoistTagSelectionsSignal)) {
                *result = 3;
            }
        }
        {
            typedef void (KitModel::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::antennaTuningSignal)) {
                *result = 4;
            }
        }
        {
            typedef void (KitModel::*_t)(FreqBandEnum );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&KitModel::bandChangedSignal)) {
                *result = 5;
            }
        }
    }
}

const QMetaObject KitModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_KitModel.data,
      qt_meta_data_KitModel,  qt_static_metacall, 0, 0}
};


const QMetaObject *KitModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *KitModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_KitModel.stringdata))
        return static_cast<void*>(const_cast< KitModel*>(this));
    return QObject::qt_metacast(_clname);
}

int KitModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void KitModel::updateTempTagsSignal(QList<SensorTag> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void KitModel::updateTempTagSelectionsSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void KitModel::updateMoistTagsSignal(QList<SensorTag> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void KitModel::updateMoistTagSelectionsSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void KitModel::antennaTuningSignal(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void KitModel::bandChangedSignal(FreqBandEnum _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
