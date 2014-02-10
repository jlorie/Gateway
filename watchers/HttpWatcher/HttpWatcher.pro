#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T09:28:53
#
#-------------------------------------------------

QT       += network
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(HttpWatcher)

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    HttpWatcher.cpp \
    HttpWatcherProvider.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    HttpWatcher.hpp \
    HttpWatcherProvider.hpp

