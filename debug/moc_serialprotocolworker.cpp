/****************************************************************************
** Meta object code from reading C++ file 'serialprotocolworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/protocol/serialprotocolworker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialprotocolworker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20SerialProtocolWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto SerialProtocolWorker::qt_create_metaobjectdata<qt_meta_tag_ZN20SerialProtocolWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SerialProtocolWorker",
        "scanPorts",
        "",
        "openConnection",
        "closeConnection",
        "sendCommand",
        "cmd",
        "setConfig",
        "SerialConfig",
        "cfg",
        "config",
        "onReadyRead",
        "onErrorOccurred",
        "QSerialPort::SerialPortError",
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
        // Slot 'setConfig'
        QtMocHelpers::SlotData<void(const SerialConfig &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Slot 'config'
        QtMocHelpers::SlotData<void() const>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onErrorOccurred'
        QtMocHelpers::SlotData<void(QSerialPort::SerialPortError)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SerialProtocolWorker, qt_meta_tag_ZN20SerialProtocolWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SerialProtocolWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<ProtocolBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20SerialProtocolWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20SerialProtocolWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20SerialProtocolWorkerE_t>.metaTypes,
    nullptr
} };

void SerialProtocolWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SerialProtocolWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->scanPorts(); break;
        case 1: _t->openConnection(); break;
        case 2: _t->closeConnection(); break;
        case 3: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 4: _t->setConfig((*reinterpret_cast<std::add_pointer_t<SerialConfig>>(_a[1]))); break;
        case 5: _t->config(); break;
        case 6: _t->onReadyRead(); break;
        case 7: _t->onErrorOccurred((*reinterpret_cast<std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *SerialProtocolWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialProtocolWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20SerialProtocolWorkerE_t>.strings))
        return static_cast<void*>(this);
    return ProtocolBase::qt_metacast(_clname);
}

int SerialProtocolWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ProtocolBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
