/****************************************************************************
** Meta object code from reading C++ file 'hermes.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "hermes.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hermes.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Hermes_t {
    QByteArrayData data[3];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Hermes_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Hermes_t qt_meta_stringdata_Hermes = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Hermes"
QT_MOC_LITERAL(1, 7, 17), // "quitButtonClicked"
QT_MOC_LITERAL(2, 25, 0) // ""

    },
    "Hermes\0quitButtonClicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Hermes[] = {

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
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Hermes::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Hermes *_t = static_cast<Hermes *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->quitButtonClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Hermes::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_Hermes.data,
      qt_meta_data_Hermes,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Hermes::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Hermes::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Hermes.stringdata0))
        return static_cast<void*>(const_cast< Hermes*>(this));
    return QWizard::qt_metacast(_clname);
}

int Hermes::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_MainPage_t {
    QByteArrayData data[5];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainPage_t qt_meta_stringdata_MainPage = {
    {
QT_MOC_LITERAL(0, 0, 8), // "MainPage"
QT_MOC_LITERAL(1, 9, 21), // "tempDemoButtonClicked"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 25), // "moistureDemoButtonClicked"
QT_MOC_LITERAL(4, 58, 28) // "remoteOperationButtonClicked"

    },
    "MainPage\0tempDemoButtonClicked\0\0"
    "moistureDemoButtonClicked\0"
    "remoteOperationButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainPage[] = {

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
       4,    0,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainPage *_t = static_cast<MainPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->tempDemoButtonClicked(); break;
        case 1: _t->moistureDemoButtonClicked(); break;
        case 2: _t->remoteOperationButtonClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject MainPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_MainPage.data,
      qt_meta_data_MainPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainPage.stringdata0))
        return static_cast<void*>(const_cast< MainPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int MainPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_TempDemoPage_t {
    QByteArrayData data[15];
    char stringdata0[291];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TempDemoPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TempDemoPage_t qt_meta_stringdata_TempDemoPage = {
    {
QT_MOC_LITERAL(0, 0, 12), // "TempDemoPage"
QT_MOC_LITERAL(1, 13, 23), // "updateTagSelectionsSlot"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 26), // "updateTempOutputLabelsSlot"
QT_MOC_LITERAL(4, 65, 16), // "QList<SensorTag>"
QT_MOC_LITERAL(5, 82, 31), // "measurementDetailsButtonClicked"
QT_MOC_LITERAL(6, 114, 23), // "mainScreenButtonClicked"
QT_MOC_LITERAL(7, 138, 17), // "helpButtonClicked"
QT_MOC_LITERAL(8, 156, 19), // "configButtonClicked"
QT_MOC_LITERAL(9, 176, 24), // "calibrationButtonClicked"
QT_MOC_LITERAL(10, 201, 13), // "periodChanged"
QT_MOC_LITERAL(11, 215, 18), // "startButtonClicked"
QT_MOC_LITERAL(12, 234, 17), // "stopButtonClicked"
QT_MOC_LITERAL(13, 252, 18), // "clearButtonClicked"
QT_MOC_LITERAL(14, 271, 19) // "exportButtonClicked"

    },
    "TempDemoPage\0updateTagSelectionsSlot\0"
    "\0updateTempOutputLabelsSlot\0"
    "QList<SensorTag>\0measurementDetailsButtonClicked\0"
    "mainScreenButtonClicked\0helpButtonClicked\0"
    "configButtonClicked\0calibrationButtonClicked\0"
    "periodChanged\0startButtonClicked\0"
    "stopButtonClicked\0clearButtonClicked\0"
    "exportButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TempDemoPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    1,   75,    2, 0x0a /* Public */,
       5,    0,   78,    2, 0x08 /* Private */,
       6,    0,   79,    2, 0x08 /* Private */,
       7,    0,   80,    2, 0x08 /* Private */,
       8,    0,   81,    2, 0x08 /* Private */,
       9,    0,   82,    2, 0x08 /* Private */,
      10,    0,   83,    2, 0x08 /* Private */,
      11,    0,   84,    2, 0x08 /* Private */,
      12,    0,   85,    2, 0x08 /* Private */,
      13,    0,   86,    2, 0x08 /* Private */,
      14,    0,   87,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TempDemoPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TempDemoPage *_t = static_cast<TempDemoPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateTagSelectionsSlot(); break;
        case 1: _t->updateTempOutputLabelsSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 2: _t->measurementDetailsButtonClicked(); break;
        case 3: _t->mainScreenButtonClicked(); break;
        case 4: _t->helpButtonClicked(); break;
        case 5: _t->configButtonClicked(); break;
        case 6: _t->calibrationButtonClicked(); break;
        case 7: _t->periodChanged(); break;
        case 8: _t->startButtonClicked(); break;
        case 9: _t->stopButtonClicked(); break;
        case 10: _t->clearButtonClicked(); break;
        case 11: _t->exportButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
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

const QMetaObject TempDemoPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_TempDemoPage.data,
      qt_meta_data_TempDemoPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TempDemoPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TempDemoPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TempDemoPage.stringdata0))
        return static_cast<void*>(const_cast< TempDemoPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int TempDemoPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
struct qt_meta_stringdata_MoistureDemoPage_t {
    QByteArrayData data[14];
    char stringdata0[271];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MoistureDemoPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MoistureDemoPage_t qt_meta_stringdata_MoistureDemoPage = {
    {
QT_MOC_LITERAL(0, 0, 16), // "MoistureDemoPage"
QT_MOC_LITERAL(1, 17, 23), // "updateTagSelectionsSlot"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 27), // "updateMoistOutputLabelsSlot"
QT_MOC_LITERAL(4, 70, 16), // "QList<SensorTag>"
QT_MOC_LITERAL(5, 87, 31), // "measurementDetailsButtonClicked"
QT_MOC_LITERAL(6, 119, 23), // "mainScreenButtonClicked"
QT_MOC_LITERAL(7, 143, 17), // "helpButtonClicked"
QT_MOC_LITERAL(8, 161, 19), // "configButtonClicked"
QT_MOC_LITERAL(9, 181, 13), // "periodChanged"
QT_MOC_LITERAL(10, 195, 18), // "startButtonClicked"
QT_MOC_LITERAL(11, 214, 17), // "stopButtonClicked"
QT_MOC_LITERAL(12, 232, 18), // "clearButtonClicked"
QT_MOC_LITERAL(13, 251, 19) // "exportButtonClicked"

    },
    "MoistureDemoPage\0updateTagSelectionsSlot\0"
    "\0updateMoistOutputLabelsSlot\0"
    "QList<SensorTag>\0measurementDetailsButtonClicked\0"
    "mainScreenButtonClicked\0helpButtonClicked\0"
    "configButtonClicked\0periodChanged\0"
    "startButtonClicked\0stopButtonClicked\0"
    "clearButtonClicked\0exportButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MoistureDemoPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    1,   70,    2, 0x0a /* Public */,
       5,    0,   73,    2, 0x08 /* Private */,
       6,    0,   74,    2, 0x08 /* Private */,
       7,    0,   75,    2, 0x08 /* Private */,
       8,    0,   76,    2, 0x08 /* Private */,
       9,    0,   77,    2, 0x08 /* Private */,
      10,    0,   78,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,
      12,    0,   80,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MoistureDemoPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MoistureDemoPage *_t = static_cast<MoistureDemoPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateTagSelectionsSlot(); break;
        case 1: _t->updateMoistOutputLabelsSlot((*reinterpret_cast< QList<SensorTag>(*)>(_a[1]))); break;
        case 2: _t->measurementDetailsButtonClicked(); break;
        case 3: _t->mainScreenButtonClicked(); break;
        case 4: _t->helpButtonClicked(); break;
        case 5: _t->configButtonClicked(); break;
        case 6: _t->periodChanged(); break;
        case 7: _t->startButtonClicked(); break;
        case 8: _t->stopButtonClicked(); break;
        case 9: _t->clearButtonClicked(); break;
        case 10: _t->exportButtonClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
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

const QMetaObject MoistureDemoPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_MoistureDemoPage.data,
      qt_meta_data_MoistureDemoPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MoistureDemoPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MoistureDemoPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MoistureDemoPage.stringdata0))
        return static_cast<void*>(const_cast< MoistureDemoPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int MoistureDemoPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
struct qt_meta_stringdata_RemoteOperationPage_t {
    QByteArrayData data[9];
    char stringdata0[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RemoteOperationPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RemoteOperationPage_t qt_meta_stringdata_RemoteOperationPage = {
    {
QT_MOC_LITERAL(0, 0, 19), // "RemoteOperationPage"
QT_MOC_LITERAL(1, 20, 15), // "outputToConsole"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 4), // "text"
QT_MOC_LITERAL(4, 42, 5), // "color"
QT_MOC_LITERAL(5, 48, 18), // "startButtonClicked"
QT_MOC_LITERAL(6, 67, 17), // "stopButtonClicked"
QT_MOC_LITERAL(7, 85, 17), // "helpButtonClicked"
QT_MOC_LITERAL(8, 103, 17) // "mainButtonClicked"

    },
    "RemoteOperationPage\0outputToConsole\0"
    "\0text\0color\0startButtonClicked\0"
    "stopButtonClicked\0helpButtonClicked\0"
    "mainButtonClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RemoteOperationPage[] = {

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
       1,    2,   39,    2, 0x0a /* Public */,
       5,    0,   44,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,
       7,    0,   46,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RemoteOperationPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RemoteOperationPage *_t = static_cast<RemoteOperationPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->outputToConsole((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->startButtonClicked(); break;
        case 2: _t->stopButtonClicked(); break;
        case 3: _t->helpButtonClicked(); break;
        case 4: _t->mainButtonClicked(); break;
        default: ;
        }
    }
}

const QMetaObject RemoteOperationPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_RemoteOperationPage.data,
      qt_meta_data_RemoteOperationPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RemoteOperationPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RemoteOperationPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RemoteOperationPage.stringdata0))
        return static_cast<void*>(const_cast< RemoteOperationPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int RemoteOperationPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
