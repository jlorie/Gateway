#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include <QTemporaryFile>
#include <QProcess>
#include <QFileSystemWatcher>

#include <QFuture>
#include <QFutureWatcher>
#include <QMap>

extern "C"
{
    #include <gammu.h>
    #include <gammu-smsd.h>
}

typedef QMap<qlonglong, qlonglong> RunningMessages;

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        QString deviceId();
        void sendMessage(const IMessage *message);
        MessageList pendingMessages() const;

        bool initialize();

    protected slots:
        void onSmsDirChanged(QString path);
        void checkSentMessages();

    private:
        bool generateConfigFile();
        bool generateDirStructure();
        qlonglong calculateMessageId(QString outfile) const;

    private:
        QString _serialPort;
        QString _number;
        QString _imei;
        QString _smsPath;

        const DeviceInfo _info;
        QTemporaryFile _configFile;

        QFileSystemWatcher _watcher;
        QFuture<GSM_Error> _loopFuture;
        QFutureWatcher<GSM_Error> _loopWatcher;

        QString _inboxPath;
        QString _outboxPath;
        QString _sentPath;

        GSM_SMSDConfig *_smsdConfig;
        RunningMessages _runningMessages;
};

#endif // DEVICE_HPP
