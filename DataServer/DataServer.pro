#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T09:51:50
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\   
    DeviceManager.cpp \
    Storage.cpp \
    DriverManager.cpp \
    SystemConfig.cpp

HEADERS  += ui/mainwindow.h \
    include/CommonErrors.hpp \
    include/IStorage.hpp \
    include/IRule.hpp \
    common/SMS.hpp \    
    DeviceManager.hpp \
    Storage.hpp \
    include/IDevice.hpp \
    include/IPhoneNumber.hpp \
    include/IMessage.hpp \         
    common/Rule.hpp \
    include/DriverInterface.hpp \
    DriverManager.hpp \
    SystemConfig.hpp \
    include/DeviceInfo.hpp
