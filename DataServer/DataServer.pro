#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T09:51:50
#
#-------------------------------------------------

include(qamqp/qamqp.pri)

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\   
    DeviceManager.cpp \
    DriverManager.cpp \
    SystemConfig.cpp \
    RemoteStorage.cpp \
    SystemEngine.cpp

HEADERS  += ui/mainwindow.h \
    include/CommonErrors.hpp \
    common/SMS.hpp \    
    DeviceManager.hpp \
    include/IDevice.hpp \
    include/IPhoneNumber.hpp \
    include/IMessage.hpp \         
    include/DriverInterface.hpp \
    DriverManager.hpp \
    SystemConfig.hpp \
    RemoteStorage.hpp \
    SystemEngine.hpp \
    include/WatcherInterface.hpp \
    include/DataStructures/DeviceInfo.hpp \
    include/DataStructures/WatcherInfo.hpp \
    include/IWatcher.hpp
