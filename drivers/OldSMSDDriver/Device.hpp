#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include <QTemporaryFile>
#include <QProcess>
#include <QFileSystemWatcher>

typedef QMap<qlonglong, qlonglong> RunningMessages;

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        QString deviceIMEI();
        void sendMessage(const IMessage *message);
        MessageList pendingMessages() const;

    protected slots:
        void readStandardOutput();
        void onSmsDirChanged(QString path);
        void checkSentMessages();

    private:
        bool generateConfigFile();
        bool generateDirStructure();
        qlonglong calculateMessageId(QString outfile) const;

    private:
        QString _serialPort;
        QString _number;

        QString _smsPath;
        QString _inboxPath;
        QString _outboxPath;
        QString _sentPath;

        const DeviceInfo _info;
        QString _configFileName;
        QTemporaryFile _configFile;

        RunningMessages _runningMessages;

        QProcess _process;
        QFileSystemWatcher _watcher;
};

#endif // DEVICE_HPP
