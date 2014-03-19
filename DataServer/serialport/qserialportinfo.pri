INCLUDEPATH += $$PWD

unix {
    greaterThan(QT_MAJOR_VERSION, 4) {
        contains(QT_CONFIG, libudev) {
            DEFINES += LINK_LIBUDEV
            INCLUDEPATH += $$QMAKE_INCDIR_LIBUDEV
            LIBS += $$QMAKE_LIBS_LIBUDEV
        }
    } else {
        packagesExist(libudev) {
            CONFIG += link_pkgconfig
            DEFINES += LINK_LIBUDEV
            PKGCONFIG += libudev
        }
    }
}

PUBLIC_HEADERS += \
    $$PWD/qserialport.h \
    $$PWD/qserialportinfo.h

PRIVATE_HEADERS += \
    $$PWD/qserialportinfo_p.h

SOURCES += \
    $$PWD/qserialportinfo.cpp

unix:!symbian {
    PRIVATE_HEADERS += \
        $$PWD/qserialport_unix_p.h

    SOURCES += \        
        $$PWD/qserialportinfo_unix.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
