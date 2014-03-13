#include "Device.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QProcess>
#include <QDir>

Device::Device(const DeviceInfo &info)
    :_info(info)
{
    _serialPort = _info.value(QString("serial_port"), QString("/dev/gsm_device"));
    _number = _info.value("device_phonenumber");
    _smsPath = info.value("sms_path");

    if (generateConfigFile())
    {
        _process.start(QString("gammu-smsd -c %1").arg(_configFileName));

        connect(&_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStandardOutput()));
        connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onInboxDirChanged()));
    }
}

Device::~Device()
{
    _process.terminate();
}

QString Device::deviceId()
{
    return _info.value(QString("device_imsi"), QString("000000000000000"));
}

void Device::sendMessage(const IMessage *message)
{
    QString command = QString(""
                              "gammu-smsd-inject "
                              "-c %1 "
                              "TEXT %2 "
                              "-len 400 "
                              "-text \"%3\"")
            .arg(_configFile.fileName(), message->to(), message->body());

    if (!QProcess::execute(command)) // error sending message
    {
        emit messageStatusChanged(message->id(), stSent);
    }
    else
    {
        emit messageStatusChanged(message->id(), stFailed);
    }
}

MessageList Device::pendingMessages() const
{
    MessageList messages;

    QDir inbox(_smsPath + "/inbox");
    foreach (QString fileName, inbox.entryList(QDir::Files|QDir::NoDotDot))
    {
        QFile file(inbox.absoluteFilePath(fileName));
        if (file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QString from = fileName.split("_").at(3);
            QString body(file.readAll().data());

            messages.append(new MessageInfo(from, _number, body));
            file.close();

            if (!file.remove())
            {
                qWarning("Error deleteting message from file %s", qPrintable(fileName));
            }

        }
        else
        {
            qWarning("Error opening new message file %s", qPrintable(fileName));
        }
    }

    return messages;
}

void Device::readStandardOutput()
{
    qWarning("%s ==> %s", qPrintable(deviceId()), qPrintable(_process.readAllStandardOutput()));
}

void Device::onInboxDirChanged()
{
    foreach (IMessage *message, pendingMessages())
    {
        emit newMessageReceived(message);
    }
}

bool Device::generateConfigFile()
{
    bool result(true);
    if (_configFile.open())
    {
        _configFileName = _configFile.fileName();

        QString strConfig = QString(""
                            "[gammu]\n "
                            "   port = %1 \n"
                            "   connection = at \n"
                            "[smsd] \n"
                            "   service = files \n"
                            "   logfile = %2/gammu.log \n"
                            "   debuglevel = 2 \n"
                            "   inboxpath = %2/inbox/ \n"
                            "   outboxpath = %2/outbox/ \n"
                            "   sentsmspath = %2/sent/ \n"
                            )
                .arg(_serialPort, _smsPath);

        _configFile.write(qPrintable(strConfig));
        _configFile.close();

        result = generateDirStructure();
        if (!result)
            qWarning("Error creating directory structure");
    }
    else
    {
        qWarning("Error generating config file");
        result = false;
    }

    return result;
}

bool Device::generateDirStructure()
{
    bool result(true);

    QDir dir;
    if (!dir.exists(_smsPath))
        result = dir.mkpath(_smsPath);

    QString inboxPath = _smsPath + "/inbox";
    if (!dir.exists(inboxPath))
        result = dir.mkpath(inboxPath);

    QString outboxPath = _smsPath + "/outbox";
    if (!dir.exists(outboxPath))
        result = dir.mkpath(outboxPath);

    QString sentPath = _smsPath + "/sent";
    if (!dir.exists(sentPath))
        result = dir.mkpath(sentPath);

    _watcher.addPath(inboxPath);

    return result;
}
