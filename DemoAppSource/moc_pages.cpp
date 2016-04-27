/****************************************************************************
** Meta object code from reading C++ file 'pages.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pages.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pages.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_RemoteOpsSettingsPage_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RemoteOpsSettingsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RemoteOpsSettingsPage_t qt_meta_stringdata_RemoteOpsSettingsPage = {
    {
QT_MOC_LITERAL(0, 0, 21) // "RemoteOpsSettingsPage"

    },
    "RemoteOpsSettingsPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RemoteOpsSettingsPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void RemoteOpsSettingsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject RemoteOpsSettingsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RemoteOpsSettingsPage.data,
      qt_meta_data_RemoteOpsSettingsPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RemoteOpsSettingsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RemoteOpsSettingsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RemoteOpsSettingsPage.stringdata0))
        return static_cast<void*>(const_cast< RemoteOpsSettingsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int RemoteOpsSettingsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_MeasurementDetailsPage_t {
    QByteArrayData data[5];
    char stringdata0[78];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MeasurementDetailsPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MeasurementDetailsPage_t qt_meta_stringdata_MeasurementDetailsPage = {
    {
QT_MOC_LITERAL(0, 0, 22), // "MeasurementDetailsPage"
QT_MOC_LITERAL(1, 23, 17), // "loadTempTableSlot"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 16), // "QList<SensorTag>"
QT_MOC_LITERAL(4, 59, 18) // "loadMoistTableSlot"

    },
    "MeasurementDetailsPage\0loadTempTableSlot\0"
    "\0QList<SensorTag>\0loadMoistTableSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MeasurementDetailsPage[] = {

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
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void MeasurementDetailsPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MeasurementDetailsPage *_t = static_cast<MeasurementDetailsPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadTempTableSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 1: _t->loadMoistTableSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
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
        }
    }
}

const QMetaObject MeasurementDetailsPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MeasurementDetailsPage.data,
      qt_meta_data_MeasurementDetailsPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MeasurementDetailsPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MeasurementDetailsPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MeasurementDetailsPage.stringdata0))
        return static_cast<void*>(const_cast< MeasurementDetailsPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int MeasurementDetailsPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_OnePointTempCalTab_t {
    QByteArrayData data[6];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_OnePointTempCalTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_OnePointTempCalTab_t qt_meta_stringdata_OnePointTempCalTab = {
    {
QT_MOC_LITERAL(0, 0, 18), // "OnePointTempCalTab"
QT_MOC_LITERAL(1, 19, 25), // "readTempCodeButtonClicked"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 21), // "writeCalButtonClicked"
QT_MOC_LITERAL(4, 68, 20), // "tempCodeMeasuredSlot"
QT_MOC_LITERAL(5, 89, 8) // "tempCode"

    },
    "OnePointTempCalTab\0readTempCodeButtonClicked\0"
    "\0writeCalButtonClicked\0tempCodeMeasuredSlot\0"
    "tempCode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_OnePointTempCalTab[] = {

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
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    1,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    5,

       0        // eod
};

void OnePointTempCalTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OnePointTempCalTab *_t = static_cast<OnePointTempCalTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->readTempCodeButtonClicked(); break;
        case 1: _t->writeCalButtonClicked(); break;
        case 2: _t->tempCodeMeasuredSlot((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject OnePointTempCalTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_OnePointTempCalTab.data,
      qt_meta_data_OnePointTempCalTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *OnePointTempCalTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *OnePointTempCalTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_OnePointTempCalTab.stringdata0))
        return static_cast<void*>(const_cast< OnePointTempCalTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int OnePointTempCalTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_TwoPointTempCalTab_t {
    QByteArrayData data[7];
    char stringdata0[126];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TwoPointTempCalTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TwoPointTempCalTab_t qt_meta_stringdata_TwoPointTempCalTab = {
    {
QT_MOC_LITERAL(0, 0, 18), // "TwoPointTempCalTab"
QT_MOC_LITERAL(1, 19, 26), // "readTempCode1ButtonClicked"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 26), // "readTempCode2ButtonClicked"
QT_MOC_LITERAL(4, 74, 21), // "writeCalButtonClicked"
QT_MOC_LITERAL(5, 96, 20), // "tempCodeMeasuredSlot"
QT_MOC_LITERAL(6, 117, 8) // "tempCode"

    },
    "TwoPointTempCalTab\0readTempCode1ButtonClicked\0"
    "\0readTempCode2ButtonClicked\0"
    "writeCalButtonClicked\0tempCodeMeasuredSlot\0"
    "tempCode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TwoPointTempCalTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    1,   37,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    6,

       0        // eod
};

void TwoPointTempCalTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TwoPointTempCalTab *_t = static_cast<TwoPointTempCalTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->readTempCode1ButtonClicked(); break;
        case 1: _t->readTempCode2ButtonClicked(); break;
        case 2: _t->writeCalButtonClicked(); break;
        case 3: _t->tempCodeMeasuredSlot((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject TwoPointTempCalTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TwoPointTempCalTab.data,
      qt_meta_data_TwoPointTempCalTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TwoPointTempCalTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TwoPointTempCalTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TwoPointTempCalTab.stringdata0))
        return static_cast<void*>(const_cast< TwoPointTempCalTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int TwoPointTempCalTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_CalibrationPage_t {
    QByteArrayData data[1];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CalibrationPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CalibrationPage_t qt_meta_stringdata_CalibrationPage = {
    {
QT_MOC_LITERAL(0, 0, 15) // "CalibrationPage"

    },
    "CalibrationPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CalibrationPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CalibrationPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CalibrationPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CalibrationPage.data,
      qt_meta_data_CalibrationPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CalibrationPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalibrationPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CalibrationPage.stringdata0))
        return static_cast<void*>(const_cast< CalibrationPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int CalibrationPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_HelpPage_t {
    QByteArrayData data[1];
    char stringdata0[9];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpPage_t qt_meta_stringdata_HelpPage = {
    {
QT_MOC_LITERAL(0, 0, 8) // "HelpPage"

    },
    "HelpPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void HelpPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject HelpPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_HelpPage.data,
      qt_meta_data_HelpPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *HelpPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_HelpPage.stringdata0))
        return static_cast<void*>(const_cast< HelpPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int HelpPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_TempDemoConfigPage_t {
    QByteArrayData data[1];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TempDemoConfigPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TempDemoConfigPage_t qt_meta_stringdata_TempDemoConfigPage = {
    {
QT_MOC_LITERAL(0, 0, 18) // "TempDemoConfigPage"

    },
    "TempDemoConfigPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TempDemoConfigPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void TempDemoConfigPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject TempDemoConfigPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TempDemoConfigPage.data,
      qt_meta_data_TempDemoConfigPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TempDemoConfigPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TempDemoConfigPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TempDemoConfigPage.stringdata0))
        return static_cast<void*>(const_cast< TempDemoConfigPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int TempDemoConfigPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_MoistureDemoConfigPage_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MoistureDemoConfigPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MoistureDemoConfigPage_t qt_meta_stringdata_MoistureDemoConfigPage = {
    {
QT_MOC_LITERAL(0, 0, 22) // "MoistureDemoConfigPage"

    },
    "MoistureDemoConfigPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MoistureDemoConfigPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void MoistureDemoConfigPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MoistureDemoConfigPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MoistureDemoConfigPage.data,
      qt_meta_data_MoistureDemoConfigPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MoistureDemoConfigPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MoistureDemoConfigPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MoistureDemoConfigPage.stringdata0))
        return static_cast<void*>(const_cast< MoistureDemoConfigPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int MoistureDemoConfigPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_TempDemoOtherSettingsTab_t {
    QByteArrayData data[10];
    char stringdata0[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TempDemoOtherSettingsTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TempDemoOtherSettingsTab_t qt_meta_stringdata_TempDemoOtherSettingsTab = {
    {
QT_MOC_LITERAL(0, 0, 24), // "TempDemoOtherSettingsTab"
QT_MOC_LITERAL(1, 25, 17), // "bandRegionChanged"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 6), // "region"
QT_MOC_LITERAL(4, 51, 28), // "samplesPerMeasurementChanged"
QT_MOC_LITERAL(5, 80, 7), // "samples"
QT_MOC_LITERAL(6, 88, 15), // "bandChangedSlot"
QT_MOC_LITERAL(7, 104, 12), // "FreqBandEnum"
QT_MOC_LITERAL(8, 117, 4), // "band"
QT_MOC_LITERAL(9, 122, 17) // "antennaTuningSlot"

    },
    "TempDemoOtherSettingsTab\0bandRegionChanged\0"
    "\0region\0samplesPerMeasurementChanged\0"
    "samples\0bandChangedSlot\0FreqBandEnum\0"
    "band\0antennaTuningSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TempDemoOtherSettingsTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       6,    1,   40,    2, 0x0a /* Public */,
       9,    2,   43,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

       0        // eod
};

void TempDemoOtherSettingsTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TempDemoOtherSettingsTab *_t = static_cast<TempDemoOtherSettingsTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->bandRegionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->samplesPerMeasurementChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->bandChangedSlot((*reinterpret_cast< FreqBandEnum(*)>(_a[1]))); break;
        case 3: _t->antennaTuningSlot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject TempDemoOtherSettingsTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TempDemoOtherSettingsTab.data,
      qt_meta_data_TempDemoOtherSettingsTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TempDemoOtherSettingsTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TempDemoOtherSettingsTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TempDemoOtherSettingsTab.stringdata0))
        return static_cast<void*>(const_cast< TempDemoOtherSettingsTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int TempDemoOtherSettingsTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_TempDemoReaderPowerTab_t {
    QByteArrayData data[7];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TempDemoReaderPowerTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TempDemoReaderPowerTab_t qt_meta_stringdata_TempDemoReaderPowerTab = {
    {
QT_MOC_LITERAL(0, 0, 22), // "TempDemoReaderPowerTab"
QT_MOC_LITERAL(1, 23, 6), // "setNum"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 1), // "v"
QT_MOC_LITERAL(4, 33, 21), // "autoPowerStateChanged"
QT_MOC_LITERAL(5, 55, 21), // "minPowerTargetChanged"
QT_MOC_LITERAL(6, 77, 21) // "maxPowerTargetChanged"

    },
    "TempDemoReaderPowerTab\0setNum\0\0v\0"
    "autoPowerStateChanged\0minPowerTargetChanged\0"
    "maxPowerTargetChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TempDemoReaderPowerTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       6,    1,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void TempDemoReaderPowerTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TempDemoReaderPowerTab *_t = static_cast<TempDemoReaderPowerTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setNum((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->autoPowerStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->minPowerTargetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->maxPowerTargetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject TempDemoReaderPowerTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TempDemoReaderPowerTab.data,
      qt_meta_data_TempDemoReaderPowerTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TempDemoReaderPowerTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TempDemoReaderPowerTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TempDemoReaderPowerTab.stringdata0))
        return static_cast<void*>(const_cast< TempDemoReaderPowerTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int TempDemoReaderPowerTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_MoistureDemoOtherSettingsTab_t {
    QByteArrayData data[16];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MoistureDemoOtherSettingsTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MoistureDemoOtherSettingsTab_t qt_meta_stringdata_MoistureDemoOtherSettingsTab = {
    {
QT_MOC_LITERAL(0, 0, 28), // "MoistureDemoOtherSettingsTab"
QT_MOC_LITERAL(1, 29, 20), // "dataReductionChanged"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 6), // "method"
QT_MOC_LITERAL(4, 58, 17), // "bandRegionChanged"
QT_MOC_LITERAL(5, 76, 6), // "region"
QT_MOC_LITERAL(6, 83, 28), // "samplesPerMeasurementChanged"
QT_MOC_LITERAL(7, 112, 7), // "samples"
QT_MOC_LITERAL(8, 120, 19), // "wetThresholdChanged"
QT_MOC_LITERAL(9, 140, 9), // "threshold"
QT_MOC_LITERAL(10, 150, 28), // "wetThresholdDirectionChanged"
QT_MOC_LITERAL(11, 179, 9), // "direction"
QT_MOC_LITERAL(12, 189, 15), // "bandChangedSlot"
QT_MOC_LITERAL(13, 205, 12), // "FreqBandEnum"
QT_MOC_LITERAL(14, 218, 4), // "band"
QT_MOC_LITERAL(15, 223, 17) // "antennaTuningSlot"

    },
    "MoistureDemoOtherSettingsTab\0"
    "dataReductionChanged\0\0method\0"
    "bandRegionChanged\0region\0"
    "samplesPerMeasurementChanged\0samples\0"
    "wetThresholdChanged\0threshold\0"
    "wetThresholdDirectionChanged\0direction\0"
    "bandChangedSlot\0FreqBandEnum\0band\0"
    "antennaTuningSlot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MoistureDemoOtherSettingsTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    1,   52,    2, 0x08 /* Private */,
       6,    1,   55,    2, 0x08 /* Private */,
       8,    1,   58,    2, 0x08 /* Private */,
      10,    1,   61,    2, 0x08 /* Private */,
      12,    1,   64,    2, 0x0a /* Public */,
      15,    2,   67,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

       0        // eod
};

void MoistureDemoOtherSettingsTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MoistureDemoOtherSettingsTab *_t = static_cast<MoistureDemoOtherSettingsTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataReductionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->bandRegionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->samplesPerMeasurementChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->wetThresholdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->wetThresholdDirectionChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->bandChangedSlot((*reinterpret_cast< FreqBandEnum(*)>(_a[1]))); break;
        case 6: _t->antennaTuningSlot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject MoistureDemoOtherSettingsTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MoistureDemoOtherSettingsTab.data,
      qt_meta_data_MoistureDemoOtherSettingsTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MoistureDemoOtherSettingsTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MoistureDemoOtherSettingsTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MoistureDemoOtherSettingsTab.stringdata0))
        return static_cast<void*>(const_cast< MoistureDemoOtherSettingsTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int MoistureDemoOtherSettingsTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
struct qt_meta_stringdata_MoistureDemoReaderPowerTab_t {
    QByteArrayData data[7];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MoistureDemoReaderPowerTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MoistureDemoReaderPowerTab_t qt_meta_stringdata_MoistureDemoReaderPowerTab = {
    {
QT_MOC_LITERAL(0, 0, 26), // "MoistureDemoReaderPowerTab"
QT_MOC_LITERAL(1, 27, 6), // "setNum"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 1), // "v"
QT_MOC_LITERAL(4, 37, 21), // "autoPowerStateChanged"
QT_MOC_LITERAL(5, 59, 21), // "minPowerTargetChanged"
QT_MOC_LITERAL(6, 81, 21) // "maxPowerTargetChanged"

    },
    "MoistureDemoReaderPowerTab\0setNum\0\0v\0"
    "autoPowerStateChanged\0minPowerTargetChanged\0"
    "maxPowerTargetChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MoistureDemoReaderPowerTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       6,    1,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void MoistureDemoReaderPowerTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MoistureDemoReaderPowerTab *_t = static_cast<MoistureDemoReaderPowerTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setNum((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->autoPowerStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->minPowerTargetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->maxPowerTargetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject MoistureDemoReaderPowerTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MoistureDemoReaderPowerTab.data,
      qt_meta_data_MoistureDemoReaderPowerTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MoistureDemoReaderPowerTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MoistureDemoReaderPowerTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MoistureDemoReaderPowerTab.stringdata0))
        return static_cast<void*>(const_cast< MoistureDemoReaderPowerTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int MoistureDemoReaderPowerTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
