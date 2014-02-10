#-------------------------------------------------
#
# Project created by QtCreator 2013-10-12T09:51:50
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = DataServer
TEMPLATE = app

CONFIG   += console

DESTDIR       = ../Gateway/bin
INSTALLS += target

SOURCES += main.cpp\   
    DeviceManager.cpp \
    DriverManager.cpp \
    SystemConfig.cpp \
    RemoteStorage.cpp \
    SystemEngine.cpp \
    WatcherManager.cpp

HEADERS  += DeviceManager.hpp \
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
