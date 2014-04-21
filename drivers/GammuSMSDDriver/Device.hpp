#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include <QTemporaryFile>

#include <QFuture>
#include <QFutureWatcher>
#include <QMap>

#include <QTimer>

extern "C"
{
    #include <gammu.h>
    #include <gammu-smsd.h>
}

typedef QMap<qlonglong, const IMessage *> RunningMessages;

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        QString deviceIMEI();
        void sendMessage(const IMessage *message);
        MessageList pendingMessages() const;

        bool initialize();

    protected slots:
        void checkSentMessages();
        void checkStatus();

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

        QFuture<GSM_Error> _loopFuture;
        QFutureWatcher<GSM_Error> _loopWatcher;
        QTimer _statusTimer;

        QString _inboxPath;
        QString _outboxPath;
        QString _sentPath;

        int _messagesReceived;
        int _messagesSent;

        GSM_SMSDConfig *_smsdConfig;
        RunningMessages _runningMessages;
};

#endif // DEVICE_HPP
