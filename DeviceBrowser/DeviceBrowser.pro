#-------------------------------------------------
#
# Project created by QtCreator 2014-01-20T10:16:29
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = DeviceManager
TEMPLATE = app

INCLUDEPATH += ../DataServer

SOURCES += main.cpp\
        MainWindow.cpp \
    ../DataServer/DriverManager.cpp \
    DeviceBrowser.cpp \
    ../DataServer/SystemConfig.cpp

HEADERS  += MainWindow.hpp \
    ../DataServer/DriverManager.hpp \
    DeviceBrowser.hpp \
    DeviceInfo.hpp \
    ../DataServer/SystemConfig.hpp

FORMS    += MainWindow.ui
