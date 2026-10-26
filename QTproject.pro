QT += core gui widgets serialport serialbus charts network

CONFIG += c++17

# Windows High-DPI 支持（Win32 平台特性）
win32 {
    QMAKE_LFLAGS += -Wl,--allow-multiple-definition
}

# 旧版 Qt 禁用 deprecated API
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += QT_CHARTS_USE_NAMESPACE

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/controller/maincontroller.cpp \
    src/core/devicedatapoint.cpp \
    src/core/databus.cpp \
    src/core/logger.cpp \
    src/protocol/protocolbase.cpp \
    src/protocol/serialprotocolworker.cpp \
    src/protocol/modbusrtuprotocolworker.cpp \
    src/protocol/modbustcpprotocolworker.cpp \
    src/protocol/canprotocolworker.cpp \
    src/protocol/simulatorworker.cpp \
    src/ui/connectionpanel.cpp \
    src/ui/datatablewidget.cpp \
    src/ui/realtimechartwidget.cpp

HEADERS += \
    src/mainwindow.h \
    src/controller/maincontroller.h \
    src/core/devicedatapoint.h \
    src/core/databus.h \
    src/core/logger.h \
    src/protocol/iprotocolworker.h \
    src/protocol/protocolbase.h \
    src/protocol/serialprotocolworker.h \
    src/protocol/modbusrtuprotocolworker.h \
    src/protocol/modbustcpprotocolworker.h \
    src/protocol/canprotocolworker.h \
    src/protocol/simulatorworker.h \
    src/ui/connectionpanel.h \
    src/ui/datatablewidget.h \
    src/ui/realtimechartwidget.h

FORMS += \
    src/mainwindow.ui

RESOURCES += \
    resources/style.qrc

# 交叉编译时安装目标到板子 /opt
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
