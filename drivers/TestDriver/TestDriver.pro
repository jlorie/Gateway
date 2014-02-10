#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin
#DEFINES += TESTDRIVER_LIBRARY
TARGET = $$qtLibraryTarget(TestDriver)

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    Device.cpp \    
    TestDriver.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/DriverInterface.hpp \
    ../../DataServer/include/IPhoneNumber.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    TestDriver.hpp

