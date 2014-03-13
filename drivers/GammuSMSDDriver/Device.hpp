#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include <QTemporaryFile>
#include <QProcess>
#include <QFileSystemWatcher>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        QString deviceId();
        void sendMessage(const IMessage *message);
        MessageList pendingMessages() const;

    protected slots:
        void readStandardOutput();
        void onInboxDirChanged();

    private:
        bool generateConfigFile();
        bool generateDirStructure();

    private:
        QString _serialPort;
        QString _number;
        QString _smsPath;

        const DeviceInfo _info;
        QString _configFileName;
        QTemporaryFile _configFile;

        QProcess _process;
        QFileSystemWatcher _watcher;
};

#endif // DEVICE_HPP
