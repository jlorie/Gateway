#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
TARGET = $$qtLibraryTarget(GammuGSMDriver)

DESTDIR       = ../../libs
INSTALLS += target

INCLUDEPATH += ../../DataServer \
                /usr/include/gammu

unix|win32: LIBS += -lGammu

SOURCES += \
    Device.cpp
    Message.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/DriverInterface.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    Message.hpp \
    DriverImp.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp

