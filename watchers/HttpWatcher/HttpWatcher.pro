#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T09:28:53
#
#-------------------------------------------------

QT       += network
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(HttpWatcher)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    HttpWatcher.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    WatcherProvider.hpp \
    HttpWatcher.hpp \

