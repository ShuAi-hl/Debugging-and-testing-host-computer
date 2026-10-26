/****************************************************************************
** Meta object code from reading C++ file 'modbustcpprotocolworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/protocol/modbustcpprotocolworker.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'modbustcpprotocolworker.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto ModbusTcpProtocolWorker::qt_create_metaobjectdata<qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ModbusTcpProtocolWorker",
        "scanPorts",
        "",
        "openConnection",
        "closeConnection",
        "sendCommand",
        "cmd",
        "writeRegister",
        "addr",
        "value",
        "setMappings",
        "QList<RegisterMapping>",
        "m",
        "mappings",
        "setConfig",
        "ModbusTcpConfig",
        "cfg",
        "config",
        "onReadyRead",
        "onSocketError",
        "QAbstractSocket::SocketError",
        "err",
        "onResponseTimeout"
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
        // Slot 'writeRegister'
        QtMocHelpers::SlotData<void(quint16, quint16)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::UShort, 8 }, { QMetaType::UShort, 9 },
        }}),
        // Slot 'setMappings'
        QtMocHelpers::SlotData<void(const QVector<RegisterMapping> &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
        // Slot 'mappings'
        QtMocHelpers::SlotData<void() const>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'setConfig'
        QtMocHelpers::SlotData<void(const ModbusTcpConfig &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'config'
        QtMocHelpers::SlotData<void() const>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onReadyRead'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'onResponseTimeout'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ModbusTcpProtocolWorker, qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ModbusTcpProtocolWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<ProtocolBase::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>.metaTypes,
    nullptr
} };

void ModbusTcpProtocolWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ModbusTcpProtocolWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->scanPorts(); break;
        case 1: _t->openConnection(); break;
        case 2: _t->closeConnection(); break;
        case 3: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 4: _t->writeRegister((*reinterpret_cast<std::add_pointer_t<quint16>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2]))); break;
        case 5: _t->setMappings((*reinterpret_cast<std::add_pointer_t<QList<RegisterMapping>>>(_a[1]))); break;
        case 6: _t->mappings(); break;
        case 7: _t->setConfig((*reinterpret_cast<std::add_pointer_t<ModbusTcpConfig>>(_a[1]))); break;
        case 8: _t->config(); break;
        case 9: _t->onReadyRead(); break;
        case 10: _t->onSocketError((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 11: _t->onResponseTimeout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 10:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject *ModbusTcpProtocolWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModbusTcpProtocolWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN23ModbusTcpProtocolWorkerE_t>.strings))
        return static_cast<void*>(this);
    return ProtocolBase::qt_metacast(_clname);
}

int ModbusTcpProtocolWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
