/****************************************************************************
** Meta object code from reading C++ file 'devicedatapoint.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/core/devicedatapoint.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'devicedatapoint.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN15DeviceDataPointE_t {};
} // unnamed namespace

template <> constexpr inline auto DeviceDataPoint::qt_create_metaobjectdata<qt_meta_tag_ZN15DeviceDataPointE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DeviceDataPoint",
        "name",
        "value",
        "unit",
        "timestamp",
        "source",
        "valid"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
        // property 'name'
        QtMocHelpers::PropertyData<QString>(1, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'value'
        QtMocHelpers::PropertyData<double>(2, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'unit'
        QtMocHelpers::PropertyData<QString>(3, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'timestamp'
        QtMocHelpers::PropertyData<QDateTime>(4, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'source'
        QtMocHelpers::PropertyData<QString>(5, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable),
        // property 'valid'
        QtMocHelpers::PropertyData<bool>(6, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DeviceDataPoint, qt_meta_tag_ZN15DeviceDataPointE_t>(QMC::PropertyAccessInStaticMetaCall, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DeviceDataPoint::staticMetaObject = { {
    nullptr,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DeviceDataPointE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN15DeviceDataPointE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN15DeviceDataPointE_t>.metaTypes,
    nullptr
} };

void DeviceDataPoint::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = reinterpret_cast<DeviceDataPoint *>(_o);
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->name; break;
        case 1: *reinterpret_cast<double*>(_v) = _t->value; break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->unit; break;
        case 3: *reinterpret_cast<QDateTime*>(_v) = _t->timestamp; break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->source; break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->valid; break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: QtMocHelpers::setProperty(_t->name, *reinterpret_cast<QString*>(_v)); break;
        case 1: QtMocHelpers::setProperty(_t->value, *reinterpret_cast<double*>(_v)); break;
        case 2: QtMocHelpers::setProperty(_t->unit, *reinterpret_cast<QString*>(_v)); break;
        case 3: QtMocHelpers::setProperty(_t->timestamp, *reinterpret_cast<QDateTime*>(_v)); break;
        case 4: QtMocHelpers::setProperty(_t->source, *reinterpret_cast<QString*>(_v)); break;
        case 5: QtMocHelpers::setProperty(_t->valid, *reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}
QT_WARNING_POP
