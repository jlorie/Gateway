#include <include/CommonErrors.hpp>

#include "Device.hpp"
#include "Message.hpp"

#include <protocol/records/IncomingSMSRecord.hpp>

const ulong TimeOut = 3000;

Device::Device(const DeviceInfo &info)
{
    _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));
    _number = info.value("device_phonenumber");

    _physical = new SerialPhysicalLayer(_serialPort);
    connect(_physical, SIGNAL(frameReceived(QString)), this, SLOT(messageReceived(QString)));
}

bool Device::initialize()
{
    //_info = info;
    bool result (false);

    result = _physical->connect(TimeOut);
    if (result)
    {
        // testing if device accepts AT commands
        {
            result = _physical->send(_message.assemble(cmAT, CommandArgs()));
            if (result)
            {
                QString data;
                result = _physical->receive(TimeOut, data);
                result = (data.contains("OK"));
            }
        }

        //setting text mode
        if (result)
        {
            CommandArgs args;
            args["mode"] = "1";

            result = _physical->send(_message.assemble(cmCMGF, args));
            if (result)
            {
                QString data;
                result = _physical->receive(TimeOut, data);
                result = (data.contains("OK"));
            }
        }

        //setting message indication
        if (result)
        {
            CommandArgs args;
            args["mode"] = "3,2,0,0,0";

            result = _physical->send(_message.assemble(cmCNMI, args));
            if (result)
            {
                QString data;
                result = _physical->receive(TimeOut, data);
                result = (data.contains("OK"));
            }
        }
    }

    return result;
}

QString Device::deviceId()
{
    //TODO determinar el IMEI por comandos AT

    return QString("0000000000000");
}

void Device::sendMessage(const IMessage *message)
{
    bool result = false;
    ATMessage atMessage;

    CommandArgs args;
    args ["number"] = QString("\"").append(message->to()).append("\"");
    args ["text"] = message->body();
    result = _physical->send(atMessage.assemble(cmCMGS, args));
    if (result)
    {
        QString data;
        result = _physical->receive(TimeOut, data);
        qDebug ("DATA = %s", qPrintable (data));
        result = (data.contains("OK"));
    }
    if (result)
    {
        qDebug ("Sent Message");
    }
    else
    {
        qDebug ("Message could't be sent");
    }
}


void Device::messageReceived(const QString &frame)
{
    qDebug("Message Received");
    AbstractRecord *newRecord = _message.disassemble(frame);

    QString receiveNumber("52501095");

    if (!newRecord)
        return;

    if (newRecord->type() == recInconmingMessage)
    {
//        qDebug("Incomming SMS: %s", qPrintable(frame));
        IncomingSMSRecord *incomingSMS = (IncomingSMSRecord *)newRecord;
        Message *newSMS = new Message(incomingSMS->from(),
                                      receiveNumber,
                                      incomingSMS->body());

        emit newMessageReceived((IMessage *)newSMS);

/////////
        bool result(_physical->send(_message.assemble(cmACK, CommandArgs())));
        if (result)
        {
            QString data;
            result = _physical->receive(TimeOut, data);
            if (result && data.contains("OK"))
            {
                qDebug("New message acknoledged");
            }
            else
            {
                qDebug("Could not acknoledge the new message");
            }
        }
        else
        {
            qDebug("Error responding: %s", qPrintable(_physical->errorString()));
        }
    }

}

