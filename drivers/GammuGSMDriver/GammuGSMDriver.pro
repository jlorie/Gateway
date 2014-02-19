#-------------------------------------------------
#
# Project created by QtCreator 2013-11-30T12:21:00
#
#-------------------------------------------------

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(GammuGSMDriver)

DESTDIR       = ../../Gateway/libs
INSTALLS += target

INCLUDEPATH += ../../DataServer \
                /usr/include/gammu

unix|win32: LIBS += -lGammu

SOURCES += \
    Device.cpp \
    GammuGSMDriver.cpp \
    MessageBuilder.cpp

HEADERS +=\
    Device.hpp \
    ../../DataServer/include/DriverInterface.hpp \
    ../../DataServer/include/IMessage.hpp \
    ../../DataServer/include/IDevice.hpp \
    ../../DataServer/include/DataStructures/MessageInfo.hpp \
    GammuGSMDriver.hpp \
    MessageBuilder.hpp

