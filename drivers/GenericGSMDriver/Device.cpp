#include <include/CommonErrors.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include "Device.hpp"
#include <protocol/records/IncomingSMSRecord.hpp>

const ulong TimeOut = 3000;
const QString countryCode("+58");

const char CZ = 0x1A;
const char CR = 0x0D;

Device::Device(const DeviceInfo &info)
{
    _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));
    _number = info.value("device_phonenumber");

    _physical = new SerialPhysicalLayer(_serialPort);
    connect(_physical, SIGNAL(frameReceived(QString)), this, SLOT(messageReceived(QString)));
}

bool Device::initialize()
{
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
            args["mode"] = "2,2,0,0,0";

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
    QString data;

    result = _physical->send(QString("AT+CMGS=\"%1\"").arg(message->to()).append(CR));
    if (result)
    {
        result = _physical->receive(1000, data);

        if (data.contains(">"))
        {
            result = _physical->send(message->body().append(CZ));
            if (result)
            {
                result = _physical->receive(3000, data);
            }
        }
    }

    if ((result = data.contains("OK")))
        emit messageStatusChanged(message->id(), stSent);
    else
        emit messageStatusChanged(message->id(), stFailed);
}


void Device::messageReceived(const QString &frame)
{    
    AbstractRecord *newRecord = _message.disassemble(frame);

    if (!newRecord)
        return;

    if (newRecord->type() == recInconmingMessage)
    {
        IncomingSMSRecord *incomingSMS = (IncomingSMSRecord *)newRecord;
        MessageInfo *newSMS = new MessageInfo(normalizeNumber(incomingSMS->from()), _number, incomingSMS->body());

        emit newMessageReceived((IMessage *)newSMS);
    }
}

QString Device::normalizeNumber(const QString &number)
{
    QString result(number);

    if (number.startsWith("0"))
        result = countryCode + number.mid(1);
    else
    if (!number.startsWith("+"))
        result.prepend(countryCode);

    return result;
}

