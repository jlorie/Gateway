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

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    AMQPWatcher.cpp \
    AMQPWatcherProvider.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    AMQPWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    AMQPWatcherProvider.hpp
