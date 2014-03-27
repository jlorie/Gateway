#-------------------------------------------------
#
# Project created by QtCreator 2014-03-27T13:37:04
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = DeviceBrowser
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += /usr/include/gammu

unix|win32: LIBS += -lGammu

SOURCES += main.cpp \
    serialport/qserialportinfo_unix.cpp \
    serialport/qserialportinfo.cpp

OTHER_FILES += \
    serialport/qserialportinfo.pri

HEADERS += \
    serialport/qserialportinfo_p.h \
    serialport/qserialportinfo.h \
    serialport/qtudev_p.h
