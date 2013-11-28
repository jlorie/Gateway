#-------------------------------------------------
#
# Project created by QtCreator 2013-10-31T11:52:29
#
#-------------------------------------------------

QT       += sql

QT       -= gui

TARGET = LiteStorage
TEMPLATE = lib

DEFINES += LITESTORAGE_LIBRARY

INCLUDEPATH += ../DataServer/include

SOURCES += LiteStorage.cpp \
    Account.cpp \
    litestorage_global.cpp

HEADERS += LiteStorage.hpp\
        litestorage_global.hpp \
        IStorage.hpp \
    Rule.hpp \
    Contact.hpp \
    Account.hpp



unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
