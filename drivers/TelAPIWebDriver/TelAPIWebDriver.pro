#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

QT       += network

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
#DEFINES += TELAPIWEBDRIVER_LIBRARY
TARGET = $$qtLibraryTarget(TelAPIWebDriver)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    Device.cpp \
    PhoneNumber.cpp \
    NetworkManager.cpp
    Message.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/IPhoneNumber.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    Message.hpp \
    PhoneNumber.hpp \
    DriverImp.hpp \
    NetworkManager.hpp \
    ../../DataServer/include/DriverInterface.hpp
