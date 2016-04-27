/****************************************************************************
** Meta object code from reading C++ file 'configdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "configdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'configdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_HelpDialog_t {
    QByteArrayData data[6];
    char stringdata0[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpDialog_t qt_meta_stringdata_HelpDialog = {
    {
QT_MOC_LITERAL(0, 0, 10), // "HelpDialog"
QT_MOC_LITERAL(1, 11, 10), // "changePage"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 40, 7), // "current"
QT_MOC_LITERAL(5, 48, 8) // "previous"

    },
    "HelpDialog\0changePage\0\0QListWidgetItem*\0"
    "current\0previous"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,

       0        // eod
};

void HelpDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpDialog *_t = static_cast<HelpDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject HelpDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_HelpDialog.data,
      qt_meta_data_HelpDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *HelpDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_HelpDialog.stringdata0))
        return static_cast<void*>(const_cast< HelpDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int HelpDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_MeasurementDetailsDialog_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MeasurementDetailsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MeasurementDetailsDialog_t qt_meta_stringdata_MeasurementDetailsDialog = {
    {
QT_MOC_LITERAL(0, 0, 24), // "MeasurementDetailsDialog"
QT_MOC_LITERAL(1, 25, 10), // "changePage"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 54, 7), // "current"
QT_MOC_LITERAL(5, 62, 8), // "previous"
QT_MOC_LITERAL(6, 71, 17) // "helpButtonClicked"

    },
    "MeasurementDetailsDialog\0changePage\0"
    "\0QListWidgetItem*\0current\0previous\0"
    "helpButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MeasurementDetailsDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       6,    0,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,

       0        // eod
};

void MeasurementDetailsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MeasurementDetailsDialog *_t = static_cast<MeasurementDetailsDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->helpButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject MeasurementDetailsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MeasurementDetailsDialog.data,
      qt_meta_data_MeasurementDetailsDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MeasurementDetailsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MeasurementDetailsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MeasurementDetailsDialog.stringdata0))
        return static_cast<void*>(const_cast< MeasurementDetailsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MeasurementDetailsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_CalibrationDialog_t {
    QByteArrayData data[7];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalibrationDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalibrationDialog_t qt_meta_stringdata_CalibrationDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CalibrationDialog"
QT_MOC_LITERAL(1, 18, 10), // "changePage"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 47, 7), // "current"
QT_MOC_LITERAL(5, 55, 8), // "previous"
QT_MOC_LITERAL(6, 64, 17) // "helpButtonClicked"

    },
    "CalibrationDialog\0changePage\0\0"
    "QListWidgetItem*\0current\0previous\0"
    "helpButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalibrationDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       6,    0,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,

       0        // eod
};

void CalibrationDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CalibrationDialog *_t = static_cast<CalibrationDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->helpButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject CalibrationDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CalibrationDialog.data,
      qt_meta_data_CalibrationDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CalibrationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalibrationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CalibrationDialog.stringdata0))
        return static_cast<void*>(const_cast< CalibrationDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int CalibrationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_ConfigTempDemoDialog_t {
    QByteArrayData data[8];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConfigTempDemoDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConfigTempDemoDialog_t qt_meta_stringdata_ConfigTempDemoDialog = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ConfigTempDemoDialog"
QT_MOC_LITERAL(1, 21, 10), // "changePage"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 50, 7), // "current"
QT_MOC_LITERAL(5, 58, 8), // "previous"
QT_MOC_LITERAL(6, 67, 11), // "closeDialog"
QT_MOC_LITERAL(7, 79, 17) // "helpButtonClicked"

    },
    "ConfigTempDemoDialog\0changePage\0\0"
    "QListWidgetItem*\0current\0previous\0"
    "closeDialog\0helpButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConfigTempDemoDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x0a /* Public */,
       6,    0,   34,    2, 0x08 /* Private */,
       7,    0,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ConfigTempDemoDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ConfigTempDemoDialog *_t = static_cast<ConfigTempDemoDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->closeDialog(); break;
        case 2: _t->helpButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject ConfigTempDemoDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConfigTempDemoDialog.data,
      qt_meta_data_ConfigTempDemoDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ConfigTempDemoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConfigTempDemoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ConfigTempDemoDialog.stringdata0))
        return static_cast<void*>(const_cast< ConfigTempDemoDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConfigTempDemoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_ConfigMoistureDemoDialog_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConfigMoistureDemoDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConfigMoistureDemoDialog_t qt_meta_stringdata_ConfigMoistureDemoDialog = {
    {
QT_MOC_LITERAL(0, 0, 24), // "ConfigMoistureDemoDialog"
QT_MOC_LITERAL(1, 25, 10), // "changePage"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(4, 54, 7), // "current"
QT_MOC_LITERAL(5, 62, 8), // "previous"
QT_MOC_LITERAL(6, 71, 17) // "helpButtonClicked"

    },
    "ConfigMoistureDemoDialog\0changePage\0"
    "\0QListWidgetItem*\0current\0previous\0"
    "helpButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConfigMoistureDemoDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x0a /* Public */,
       6,    0,   29,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,

       0        // eod
};

void ConfigMoistureDemoDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ConfigMoistureDemoDialog *_t = static_cast<ConfigMoistureDemoDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->helpButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject ConfigMoistureDemoDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ConfigMoistureDemoDialog.data,
      qt_meta_data_ConfigMoistureDemoDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ConfigMoistureDemoDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConfigMoistureDemoDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ConfigMoistureDemoDialog.stringdata0))
        return static_cast<void*>(const_cast< ConfigMoistureDemoDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ConfigMoistureDemoDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
