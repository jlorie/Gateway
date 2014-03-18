#include "Device.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QtConcurrentRun>
#include <QProcess>
#include <QDir>

#include <fstream>

Device::Device(const DeviceInfo &info)
    :_info(info)
{
    _serialPort = _info.value(QString("serial_port"), QString("/dev/gsm_device"));
    _number = _info.value("device_phonenumber");
    _smsPath = info.value("sms_path");
}

Device::~Device()
{
    GSM_Error error;
    error = SMSD_Shutdown(_smsdConfig);
    if (error != ERR_NONE)
    {
        qWarning("Error shutting down sms daemon");
        return;
    }

    SMSD_FreeConfig(_smsdConfig);
}

QString Device::deviceId()
{
    return _info.value(QString("device_imsi"), QString("000000000000000"));
}


bool Device::initialize()
{
    bool result(true);
    if (generateConfigFile())
    {
        GSM_Error error;
        GSM_InitLocales(NULL);

        /* Initalize configuration with program name */
        _smsdConfig = SMSD_NewConfig(qPrintable(deviceId()));

        /* Read configuration file */
        error = SMSD_ReadConfig(qPrintable(_configFile.fileName()), _smsdConfig, TRUE);
        if (error != ERR_NONE) {
            qWarning("Failed to read config!");
            SMSD_FreeConfig(_smsdConfig);

            result = false;
        }

        if (result)
        {
            _loopFuture = QtConcurrent::run(SMSD_MainLoop, _smsdConfig, TRUE, 10);

            connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onSmsDirChanged(QString)));
        }
    }
    else
    {
        result = false;
    }

    return result;
}

void Device::sendMessage(const IMessage *message)
{
    GSM_Error error(ERR_NONE);
    GSM_MultiSMSMessage SMS;

    //Preparing gammu-message
    {
        GSM_MultiPartSMSInfo SMSInfo;
        unsigned char message_unicode[(message->body().length() + 1) * 2];

        GSM_ClearMultiPartSMSInfo(&SMSInfo);
        SMSInfo.Class = 1;
        SMSInfo.EntriesNum = 1;
        SMSInfo.UnicodeCoding = FALSE;
        SMSInfo.Entries[0].ID = SMS_ConcatenatedTextLong;
        EncodeUnicode(message_unicode, qPrintable(message->body()), message->body().length());
        SMSInfo.Entries[0].Buffer = message_unicode;

        error = GSM_EncodeMultiPartSMS(NULL, &SMSInfo, &SMS);
        if (error != ERR_NONE)
        {
            qWarning ("Error encoding MultiPart message (%d): %s", error, GSM_ErrorString(error));
        }

        for (int i = 0; i < SMS.Number; i++)
        {
            EncodeUnicode(SMS.SMS[i].Number, qPrintable(message->to()), message->to().length());
            SMS.SMS[i].PDU = SMS_Submit;
        }
    }

    char *outFileName = new char[200];
    error = SMSD_InjectSMS(_smsdConfig, &SMS, outFileName);

    if (error != ERR_NONE)
    {
        emit messageStatusChanged(message->id(), stFailed);
    }
    else
    {
//        if (!_loopFuture.isRunning())
//            QString out(outFileName);
//            QString gammuId = out.mid(out.lastIndexOf("/") + 1);

//            _runningMessages.insert(gammuId, message->id());
        emit messageStatusChanged(message->id(), stSent);
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
//        checkSentMessages();
    }
    else
    if (path == _outboxPath)
    {

    }
}

void Device::checkSentMessages()
{
    QDir inbox(_sentPath);
    foreach (QString fileName, inbox.entryList(QDir::Files|QDir::NoDotDot))
    {
        qlonglong messageId = _runningMessages.take(fileName);

        if (messageId)
        {
            emit messageStatusChanged(messageId, stSent);

            if (!inbox.remove(fileName))
            {
                qWarning("Error deleteting message from file %s", qPrintable(fileName));
            }
        }
    }
}

bool Device::generateConfigFile()
{
    bool result(true);
    if (_configFile.open())
    {
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

    return result;
}
