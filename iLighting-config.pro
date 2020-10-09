QT       += core gui serialbus widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets sql

qtConfig(modbus-serialport): QT += serialport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adc.cpp \
    devicetreedock.cpp \
    gatewaybasictabwidget.cpp \
    global_res.cpp \
    iic1.cpp \
    logmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    relaypropertytabwidget.cpp \
    settingsdialog.cpp \
    statusfield.cpp \
    touchpaneltabwidget.cpp

HEADERS += \
    adc.h \
    device_common.h \
    devicetreedock.h \
    gateway_module_conf.h \
    gatewaybasictabwidget.h \
    global_res.h \
    iic1.h \
    logmanager.h \
    mainwindow.h \
    relay_module_conf.h \
    relaypropertytabwidget.h \
    settingsdialog.h \
    statusfield.h \
    touchpanel_conf.h \
    touchpaneltabwidget.h

FORMS += \
    adc.ui \
    devicetreedock.ui \
    gatewaybasictabwidget.ui \
    iic1.ui \
    mainwindow.ui \
    relaypropertytabwidget.ui \
    settingsdialog.ui \
    statusfield.ui \
    touchpaneltabwidget.ui

TRANSLATIONS += iLighting-conifg_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#RESOURCES += \
#resources.qrc

RC_ICONS = myico.ico

RESOURCES += \
    resources.qrc
