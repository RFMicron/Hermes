/****************************************************************************
** Meta object code from reading C++ file 'chart.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "chart.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chart.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Chart_t {
    QByteArrayData data[14];
    char stringdata[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Chart_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Chart_t qt_meta_stringdata_Chart = {
    {
QT_MOC_LITERAL(0, 0, 5),
QT_MOC_LITERAL(1, 6, 17),
QT_MOC_LITERAL(2, 24, 0),
QT_MOC_LITERAL(3, 25, 16),
QT_MOC_LITERAL(4, 42, 11),
QT_MOC_LITERAL(5, 54, 18),
QT_MOC_LITERAL(6, 73, 12),
QT_MOC_LITERAL(7, 86, 20),
QT_MOC_LITERAL(8, 107, 16),
QT_MOC_LITERAL(9, 124, 16),
QT_MOC_LITERAL(10, 141, 21),
QT_MOC_LITERAL(11, 163, 17),
QT_MOC_LITERAL(12, 181, 13),
QT_MOC_LITERAL(13, 195, 2)
    },
    "Chart\0setTempCurvesSlot\0\0QList<SensorTag>\0"
    "TempTagList\0setMoistCurvesSlot\0"
    "MoistTagList\0updateTempCurvesSlot\0"
    "QList<QDateTime>\0TempMeasTimeList\0"
    "updateMoistCurvesSlot\0MoistMeasTimeList\0"
    "legendChecked\0on"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Chart[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       5,    1,   42,    2, 0x0a /* Public */,
       7,    2,   45,    2, 0x0a /* Public */,
      10,    2,   50,    2, 0x0a /* Public */,
      12,    2,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 8,    4,    9,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 8,    6,   11,
    QMetaType::Void, QMetaType::QVariant, QMetaType::Bool,    2,   13,

       0        // eod
};

void Chart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Chart *_t = static_cast<Chart *>(_o);
        switch (_id) {
        case 0: _t->setTempCurvesSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 1: _t->setMoistCurvesSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 2: _t->updateTempCurvesSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1])),(*reinterpret_cast< QList<QDateTime>(*)>(_a[2]))); break;
        case 3: _t->updateMoistCurvesSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1])),(*reinterpret_cast< QList<QDateTime>(*)>(_a[2]))); break;
        case 4: _t->legendChecked((*reinterpret_cast< const QVariant(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
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
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<SensorTag> >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QDateTime> >(); break;
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
    }
}

const QMetaObject Chart::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_Chart.data,
      qt_meta_data_Chart,  qt_static_metacall, 0, 0}
};


const QMetaObject *Chart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Chart::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Chart.stringdata))
        return static_cast<void*>(const_cast< Chart*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int Chart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
