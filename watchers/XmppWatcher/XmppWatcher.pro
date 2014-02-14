#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T09:28:53
#
#-------------------------------------------------

QT       += network
QT       += xml
TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(XmppWatcher)

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    XmppWatcher.cpp \
    XmppWatcherProvider.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    XmppWatcher.hpp \
    XmppWatcherProvider.hpp

unix|win32: LIBS += -lqxmpp
