#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

QT       += serialport

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
#DEFINES += MOTOROLAGSMDRIVER_LIBRARY
TARGET = $$qtLibraryTarget(MotorolaGSMDriver)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    Device.cpp \
    PhoneNumber.cpp \
    physical/SerialPhysicalLayer.cpp \
    protocol/ATMessage.cpp \
    Message.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/IPhoneNumber.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    Message.hpp \
    PhoneNumber.hpp \
    physical/SerialPhysicalLayer.hpp \
    ../DataServer/include/CommonErrors.hpp \
    protocol/ATMessage.hpp \
    protocol/ATCommand.hpp \
    protocol/records/IncomingSMSRecord.hpp \
    protocol/records/AbstractRecord.hpp \
    Utils.hpp \
    DriverImp.hpp
