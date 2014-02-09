#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T09:51:50
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataServer
TEMPLATE = app

SOURCES += main.cpp\   
    DeviceManager.cpp \
    DriverManager.cpp \
    SystemConfig.cpp \
    RemoteStorage.cpp \
    SystemEngine.cpp \
    WatcherManager.cpp

HEADERS  += ui/mainwindow.h \
    DeviceManager.hpp \
    include/IDevice.hpp \
    include/IMessage.hpp \         
    include/DriverInterface.hpp \
    DriverManager.hpp \
    SystemConfig.hpp \
    RemoteStorage.hpp \
    SystemEngine.hpp \
    include/WatcherInterface.hpp \
    include/DataStructures/DeviceInfo.hpp \
    include/DataStructures/WatcherInfo.hpp \
    include/IWatcher.hpp \
    include/DataStructures/MessageInfo.hpp \
    include/DataStructures/MessageList.hpp \
    include/DataStructures/MainInfo.hpp \
    WatcherManager.hpp \
    include/DataStructures/NumberInfo.hpp

unix|win32: LIBS += -lqjson
