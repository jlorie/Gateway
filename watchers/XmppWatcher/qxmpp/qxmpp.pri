QT += network xml

INCLUDEPATH += $$PWD/base $$PWD/client

DEPENDPATH += $$PWD

#----------Base--------------------------------------------------
# Header files
INSTALL_HEADERS += \
    $$PWD/base/QXmppArchiveIq.h \
    $$PWD/base/QXmppBindIq.h \
    $$PWD/base/QXmppBookmarkSet.h \
    $$PWD/base/QXmppByteStreamIq.h \
    $$PWD/base/QXmppConstants.h \
    $$PWD/base/QXmppDataForm.h \
    $$PWD/base/QXmppDiscoveryIq.h \
    $$PWD/base/QXmppElement.h \
    $$PWD/base/QXmppEntityTimeIq.h \
    $$PWD/base/QXmppGlobal.h \
    $$PWD/base/QXmppIbbIq.h \
    $$PWD/base/QXmppIq.h \
    $$PWD/base/QXmppJingleIq.h \
    $$PWD/base/QXmppLogger.h \
    $$PWD/base/QXmppMessage.h \
    $$PWD/base/QXmppMucIq.h \
    $$PWD/base/QXmppNonSASLAuth.h \
    $$PWD/base/QXmppPingIq.h \
    $$PWD/base/QXmppPresence.h \
    $$PWD/base/QXmppPubSubIq.h \
    $$PWD/base/QXmppRegisterIq.h \
    $$PWD/base/QXmppResultSet.h \
    $$PWD/base/QXmppRosterIq.h \
    $$PWD/base/QXmppRpcIq.h \
    $$PWD/base/QXmppRtpChannel.h \
    $$PWD/base/QXmppSessionIq.h \
    $$PWD/base/QXmppSocks.h \
    $$PWD/base/QXmppStanza.h \
    $$PWD/base/QXmppStream.h \
    $$PWD/base/QXmppStreamFeatures.h \
    $$PWD/base/QXmppStun.h \
    $$PWD/base/QXmppUtils.h \
    $$PWD/base/QXmppVCardIq.h \
    $$PWD/base/QXmppVersionIq.h

HEADERS += \
    $$PWD/base/QXmppCodec_p.h \
    $$PWD/base/QXmppSasl_p.h \
    $$PWD/base/QXmppStreamInitiationIq_p.h

# Source files
SOURCES += \
    $$PWD/base/QXmppArchiveIq.cpp \
    $$PWD/base/QXmppBindIq.cpp \
    $$PWD/base/QXmppBookmarkSet.cpp \
    $$PWD/base/QXmppByteStreamIq.cpp \
    $$PWD/base/QXmppCodec.cpp \
    $$PWD/base/QXmppConstants.cpp \
    $$PWD/base/QXmppDataForm.cpp \
    $$PWD/base/QXmppDiscoveryIq.cpp \
    $$PWD/base/QXmppElement.cpp \
    $$PWD/base/QXmppEntityTimeIq.cpp \
    $$PWD/base/QXmppGlobal.cpp \
    $$PWD/base/QXmppIbbIq.cpp \
    $$PWD/base/QXmppIq.cpp \
    $$PWD/base/QXmppJingleIq.cpp \
    $$PWD/base/QXmppLogger.cpp \
    $$PWD/base/QXmppMessage.cpp \
    $$PWD/base/QXmppMucIq.cpp \
    $$PWD/base/QXmppNonSASLAuth.cpp \
    $$PWD/base/QXmppPingIq.cpp \
    $$PWD/base/QXmppPresence.cpp \
    $$PWD/base/QXmppPubSubIq.cpp \
    $$PWD/base/QXmppRegisterIq.cpp \
    $$PWD/base/QXmppResultSet.cpp \
    $$PWD/base/QXmppRosterIq.cpp \
    $$PWD/base/QXmppRpcIq.cpp \
    $$PWD/base/QXmppRtpChannel.cpp \
    $$PWD/base/QXmppSasl.cpp \
    $$PWD/base/QXmppSessionIq.cpp \
    $$PWD/base/QXmppSocks.cpp \
    $$PWD/base/QXmppStanza.cpp \
    $$PWD/base/QXmppStream.cpp \
    $$PWD/base/QXmppStreamFeatures.cpp \
    $$PWD/base/QXmppStreamInitiationIq.cpp \
    $$PWD/base/QXmppStun.cpp \
    $$PWD/base/QXmppUtils.cpp \
    $$PWD/base/QXmppVCardIq.cpp \
    $$PWD/base/QXmppVersionIq.cpp

# DNS
qt_version = $$QT_MAJOR_VERSION
contains(qt_version, 4) {
    INSTALL_HEADERS += $$PWD/base/qdnslookup.h $$PWD/base/qdnslookup_p.h
    SOURCES += $$PWD/base/qdnslookup.cpp
    android:SOURCES += $$PWD/base/qdnslookup_stub.cpp
    else:symbian:SOURCES += $$PWD/base/qdnslookup_symbian.cpp
    else:unix:SOURCES += $$PWD/base/qdnslookup_unix.cpp
    else:win32:SOURCES += $$PWD/base/qdnslookup_win.cpp
    else:SOURCES += $$PWD/base/qdnslookup_stub.cpp
}

#----------$$PWD/client/--------------------------------------------------
# Header files
INSTALL_HEADERS += \
    $$PWD/client//QXmppArchiveManager.h \
    $$PWD/client//QXmppBookmarkManager.h \
    $$PWD/client//QXmppCallManager.h \
    $$PWD/client//QXmppClient.h \
    $$PWD/client//QXmppClientExtension.h \
    $$PWD/client//QXmppConfiguration.h \
    $$PWD/client//QXmppDiscoveryManager.h \
    $$PWD/client//QXmppEntityTimeManager.h \
    $$PWD/client//QXmppInvokable.h \
    $$PWD/client//QXmppMessageReceiptManager.h \
    $$PWD/client//QXmppMucManager.h \
    $$PWD/client//QXmppOutgoingClient.h \
    $$PWD/client//QXmppRemoteMethod.h \
    $$PWD/client//QXmppRosterManager.h \
    $$PWD/client//QXmppRpcManager.h \
    $$PWD/client//QXmppTransferManager.h \
    $$PWD/client//QXmppTransferManager_p.h \
    $$PWD/client//QXmppVCardManager.h \
    $$PWD/client//QXmppVersionManager.h

# Source files
SOURCES += \
    $$PWD/client//QXmppDiscoveryManager.cpp \
    $$PWD/client//QXmppArchiveManager.cpp \
    $$PWD/client//QXmppBookmarkManager.cpp \
    $$PWD/client//QXmppCallManager.cpp \
    $$PWD/client//QXmppClient.cpp \
    $$PWD/client//QXmppClientExtension.cpp \
    $$PWD/client//QXmppConfiguration.cpp \
    $$PWD/client//QXmppEntityTimeManager.cpp \
    $$PWD/client//QXmppInvokable.cpp \
    $$PWD/client//QXmppMessageReceiptManager.cpp \
    $$PWD/client//QXmppMucManager.cpp \
    $$PWD/client//QXmppOutgoingClient.cpp \
    $$PWD/client//QXmppRemoteMethod.cpp \
    $$PWD/client//QXmppRosterManager.cpp \
    $$PWD/client//QXmppRpcManager.cpp \
    $$PWD/client//QXmppTransferManager.cpp \
    $$PWD/client//QXmppVCardManager.cpp \
    $$PWD/client//QXmppVersionManager.cpp

HEADERS += $$INSTALL_HEADERS

