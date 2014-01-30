#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

QT       += serialport core

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
#DEFINES += GENERICGSMDRIVER_LIBRARY
TARGET = $$qtLibraryTarget(GenericGSMDriver)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer

SOURCES += \
    Device.cpp \
    PhoneNumber.cpp \
    physical/SerialPhysicalLayer.cpp \
    protocol/ATMessage.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/IPhoneNumber.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    PhoneNumber.hpp \
    physical/SerialPhysicalLayer.hpp \
    ../DataServer/include/CommonErrors.hpp \
    protocol/ATMessage.hpp \
    protocol/ATCommand.hpp \
    protocol/records/IncomingSMSRecord.hpp \
    protocol/records/AbstractRecord.hpp \
    Utils.hpp \
    DriverImp.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp
