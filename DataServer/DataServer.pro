#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T09:51:50
#
#-------------------------------------------------

QT       += core gui network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
    ui/mainwindow.cpp \
    devices/AbstractGSMDevice.cpp \    
    devices/SerialGSMDevice.cpp \
    webapi/TelAPISMS.cpp \
    DeviceManager.cpp \
    devices/physical/SerialPhysicalLayer.cpp \
    devices/protocol/ATMessage.cpp \
    AccountManager.cpp \
    storage/Storage.cpp \
    storage/LocalStorage.cpp \
    storage/RemoteStorage.cpp \
    ui/Dialog.cpp \
    ui/RemoveRuleDialog.cpp

HEADERS  += ui/mainwindow.h \
    common/CommonErrors.hpp \
    common/SMS.hpp \
    devices/DeviceInfo.hpp \
    devices/AbstractGSMDevice.hpp \    
    devices/SerialGSMDevice.hpp \    
    devices/physical/SerialPhysicalLayer.hpp \
    devices/physical/IPhysicalLayer.hpp \
    devices/protocol/records/IncomingSMSRecord.hpp \
    devices/protocol/ATMessage.hpp \
    devices/protocol/ATCommand.hpp \
    devices/protocol/AbstractRecord.hpp \
    devices/protocol/records/AbstractRecord.hpp \
    webapi/TelAPISMS.hpp \
    DeviceManager.hpp \
    AccountManager.hpp \    
    include/IStorage.hpp \
    include/IRule.hpp \
    include/IContact.hpp \
    include/IAccount.hpp \
    storage/Storage.hpp \
    storage/LocalStorage.hpp \
    storage/RemoteStorage.hpp \
    ui/Dialog.hpp \
    ui/RemoveRuleDialog.hpp \
    IPhone.hpp
FORMS    += ui/mainwindow.ui \
    ui/Dialog.ui \
    ui/RemoveRuleDialog.ui
