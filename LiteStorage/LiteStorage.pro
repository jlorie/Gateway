#-------------------------------------------------
#
# Project created by QtCreator 2013-10-31T11:52:29
#
#-------------------------------------------------

QT       += sql

TEMPLATE = lib
CONFIG += plugin
DEFINES += LITESTORAGE_LIBRARY
TARGET = $$qtLibraryTarget(LiteStorage)

DESTDIR       = ../libs
INSTALLS += target

INCLUDEPATH += ../DataServer

SOURCES += LiteStorage.cpp

HEADERS += LiteStorage.hpp\
        IStorage.hpp \
    Rule.hpp \
    include/IStorage.hpp \
    include/IRule.hpp \
    include/IContact.hpp \
    include/IAccount.hpp \
    ../DataServer/include/IRule.hpp
