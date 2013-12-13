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
    webapi/TelAPISMS.cpp \
    DeviceManager.cpp \
    Storage.cpp

HEADERS  += ui/mainwindow.h \
    include/CommonErrors.hpp \
    include/IStorage.hpp \
    include/IRule.hpp \
    common/SMS.hpp \
    webapi/TelAPISMS.hpp \
    DeviceManager.hpp \
    Storage.hpp \
    include/IDevice.hpp \
    include/IPhoneNumber.hpp \
    include/IMessage.hpp \         
    common/Rule.hpp
