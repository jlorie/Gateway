#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T09:28:53
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin

include(qxmpp/qxmpp.pri)

TARGET = $$qtLibraryTarget(XmppWatcher)

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer \
                qxmpp

SOURCES += \
    XmppWatcher.cpp \
    XmppWatcherProvider.cpp

HEADERS +=\
    ../../DataServer/include/WatcherInterface.hpp \
    ../../DataServer/include/IWatcher.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    XmppWatcher.hpp \
    XmppWatcherProvider.hpp
