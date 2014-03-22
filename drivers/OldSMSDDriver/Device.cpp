#include "Device.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QProcess>
#include <QDir>
#include <QMetaType>

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
        connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onSmsDirChanged(QString)));

        qRegisterMetaType<MessageStatus>("MessageStatus");
    }
}

Device::~Device()
{
    _process.terminate();
}

QString Device::deviceIMEI()
{
    return _info.value(QString("device_imei"), QString("000000000000000"));
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

    QProcess injectCommand;
    injectCommand.start(command);
    if (injectCommand.waitForFinished(3 * 1000)) // 3s
    {
        QString output(injectCommand.readAllStandardOutput().data());
        qlonglong gammuId = calculateMessageId(output.mid(output.lastIndexOf("/") + 1));

        _runningMessages.insert(gammuId, message->id());
    }
    else
    {
        emit messageStatusChanged(message->id(), stFailed);
    }
}

MessageList Device::pendingMessages() const
{
    MessageList messages;

    QDir inbox(_inboxPath);
    foreach (QString fileName, inbox.entryList(QDir::Files|QDir::NoDotDot))
    {
        QFile file(inbox.absoluteFilePath(fileName));
        if (file.size() == 0)
            continue;

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

void Device::checkSentMessages()
{
    QDir sentbox(_sentPath);
    foreach (QString fileName, sentbox.entryList(QDir::Files|QDir::NoDotDot))
    {
        QFile file(sentbox.absoluteFilePath(fileName));
        if (file.size() == 0)
            continue;

        qlonglong messageId = _runningMessages.take(calculateMessageId(fileName));

        if (messageId)
        {
            emit messageStatusChanged(messageId, stSent);

            if (!sentbox.remove(fileName))
            {
                qWarning("Error deleteting message from file %s", qPrintable(fileName));
            }
        }
    }
}

void Device::readStandardOutput()
{
    qWarning("%s", qPrintable(_process.readAllStandardOutput()));
}

void Device::onSmsDirChanged(QString path)
{
    if (path == _inboxPath)
    {
        foreach (IMessage *message, pendingMessages())
        {
            emit newMessageReceived(message);
        }
    }
    else
    if (path == _sentPath)
    {
        checkSentMessages();
    }
    else
    if (path == _outboxPath)
    {

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

    _inboxPath = _smsPath + "/inbox";
    if (!dir.exists(_inboxPath))
        result = dir.mkpath(_inboxPath);

    _outboxPath = _smsPath + "/outbox";
    if (!dir.exists(_outboxPath))
        result = dir.mkpath(_outboxPath);

    _sentPath = _smsPath + "/sent";
    if (!dir.exists(_sentPath))
        result = dir.mkpath(_sentPath);

    _watcher.addPath(_inboxPath);
    _watcher.addPath(_sentPath);

    return result;
}

qlonglong Device::calculateMessageId(QString outfile) const
{
    QString fileName = outfile.mid(outfile.lastIndexOf("/") + 1);
    QString id = fileName.split("_").at(1);
    return id.toLongLong();
}
