#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T09:28:53
#
#-------------------------------------------------

include(qamqp/qamqp.pri)

QT       += network
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(AMQPWatcher)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    AMQPWatcher.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    WatcherProvider.hpp \
    AMQPWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp
