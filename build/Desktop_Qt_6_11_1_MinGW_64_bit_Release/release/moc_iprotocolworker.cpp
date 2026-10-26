/****************************************************************************
** Meta object code from reading C++ file 'iprotocolworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/protocol/iprotocolworker.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'iprotocolworker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15IProtocolWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto IProtocolWorker::qt_create_metaobjectdata<qt_meta_tag_ZN15IProtocolWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "IProtocolWorker",
        "portsScanned",
        "",
        "ports",
        "connectionStateChanged",
        "connected",
        "errorOccurred",
        "message",
        "dataParsed",
        "DeviceDataPoint",
        "dp",
        "batchDataParsed",
        "QList<DeviceDataPoint>",
        "list",
        "rawBytesReceived",
        "data",
        "scanPorts",
        "openConnection",
        "closeConnection",
        "sendCommand",
        "cmd"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'portsScanned'
        QtMocHelpers::SignalData<void(const QStringList &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 3 },
        }}),
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'dataParsed'
        QtMocHelpers::SignalData<void(const DeviceDataPoint &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Signal 'batchDataParsed'
        QtMocHelpers::SignalData<void(const QList<DeviceDataPoint> &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Signal 'rawBytesReceived'
        QtMocHelpers::SignalData<void(const QByteArray &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 15 },
        }}),
        // Slot 'scanPorts'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'openConnection'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'closeConnection'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendCommand'
        QtMocHelpers::SlotData<void(const QByteArray &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 20 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<IProtocolWorker, qt_meta_tag_ZN15IProtocolWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject IProtocolWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15IProtocolWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15IProtocolWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15IProtocolWorkerE_t>.metaTypes,
    nullptr
} };

void IProtocolWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<IProtocolWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->portsScanned((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 1: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->dataParsed((*reinterpret_cast<std::add_pointer_t<DeviceDataPoint>>(_a[1]))); break;
        case 4: _t->batchDataParsed((*reinterpret_cast<std::add_pointer_t<QList<DeviceDataPoint>>>(_a[1]))); break;
        case 5: _t->rawBytesReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 6: _t->scanPorts(); break;
        case 7: _t->openConnection(); break;
        case 8: _t->closeConnection(); break;
        case 9: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< DeviceDataPoint >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<DeviceDataPoint> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(const QStringList & )>(_a, &IProtocolWorker::portsScanned, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(bool )>(_a, &IProtocolWorker::connectionStateChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(const QString & )>(_a, &IProtocolWorker::errorOccurred, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(const DeviceDataPoint & )>(_a, &IProtocolWorker::dataParsed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(const QList<DeviceDataPoint> & )>(_a, &IProtocolWorker::batchDataParsed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (IProtocolWorker::*)(const QByteArray & )>(_a, &IProtocolWorker::rawBytesReceived, 5))
            return;
    }
}

const QMetaObject *IProtocolWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *IProtocolWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15IProtocolWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int IProtocolWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void IProtocolWorker::portsScanned(const QStringList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void IProtocolWorker::connectionStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void IProtocolWorker::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void IProtocolWorker::dataParsed(const DeviceDataPoint & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void IProtocolWorker::batchDataParsed(const QList<DeviceDataPoint> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void IProtocolWorker::rawBytesReceived(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
