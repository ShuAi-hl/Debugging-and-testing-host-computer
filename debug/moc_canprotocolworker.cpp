/****************************************************************************
** Meta object code from reading C++ file 'canprotocolworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/protocol/canprotocolworker.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'canprotocolworker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17CanProtocolWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto CanProtocolWorker::qt_create_metaobjectdata<qt_meta_tag_ZN17CanProtocolWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CanProtocolWorker",
        "scanPorts",
        "",
        "openConnection",
        "closeConnection",
        "sendCommand",
        "cmd",
        "sendCanFrame",
        "id",
        "payload",
        "extended",
        "setConfig",
        "CanConfig",
        "cfg",
        "config",
        "setFrameMappings",
        "QList<CanFrameMapping>",
        "m",
        "frameMappings",
        "onFramesReceived",
        "onDeviceError",
        "QCanBusDevice::CanBusError",
        "err"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'scanPorts'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'openConnection'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'closeConnection'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendCommand'
        QtMocHelpers::SlotData<void(const QByteArray &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 6 },
        }}),
        // Slot 'sendCanFrame'
        QtMocHelpers::SlotData<void(quint32, const QByteArray &, bool)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UInt, 8 }, { QMetaType::QByteArray, 9 }, { QMetaType::Bool, 10 },
        }}),
        // Slot 'sendCanFrame'
        QtMocHelpers::SlotData<void(quint32, const QByteArray &)>(7, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::UInt, 8 }, { QMetaType::QByteArray, 9 },
        }}),
        // Slot 'setConfig'
        QtMocHelpers::SlotData<void(const CanConfig &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Slot 'config'
        QtMocHelpers::SlotData<void() const>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setFrameMappings'
        QtMocHelpers::SlotData<void(const QVector<CanFrameMapping> &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Slot 'frameMappings'
        QtMocHelpers::SlotData<void() const>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onFramesReceived'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDeviceError'
        QtMocHelpers::SlotData<void(QCanBusDevice::CanBusError)>(20, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 21, 22 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CanProtocolWorker, qt_meta_tag_ZN17CanProtocolWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CanProtocolWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<ProtocolBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CanProtocolWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CanProtocolWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17CanProtocolWorkerE_t>.metaTypes,
    nullptr
} };

void CanProtocolWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CanProtocolWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->scanPorts(); break;
        case 1: _t->openConnection(); break;
        case 2: _t->closeConnection(); break;
        case 3: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 4: _t->sendCanFrame((*reinterpret_cast<std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 5: _t->sendCanFrame((*reinterpret_cast<std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 6: _t->setConfig((*reinterpret_cast<std::add_pointer_t<CanConfig>>(_a[1]))); break;
        case 7: _t->config(); break;
        case 8: _t->setFrameMappings((*reinterpret_cast<std::add_pointer_t<QList<CanFrameMapping>>>(_a[1]))); break;
        case 9: _t->frameMappings(); break;
        case 10: _t->onFramesReceived(); break;
        case 11: _t->onDeviceError((*reinterpret_cast<std::add_pointer_t<QCanBusDevice::CanBusError>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *CanProtocolWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CanProtocolWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17CanProtocolWorkerE_t>.strings))
        return static_cast<void*>(this);
    return ProtocolBase::qt_metacast(_clname);
}

int CanProtocolWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ProtocolBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
