#include <include/CommonErrors.hpp>

#include "Device.hpp"
#include "PhoneNumber.hpp"
#include "Message.hpp"

#include <protocol/records/IncomingSMSRecord.hpp>

const ulong TimeOut = 3000;
const QString countryCode("+58");


Device::Device(const DeviceInfo &info)
{
    _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));

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

    if (result)
    {
        _numbers.append(new PhoneNumber(_physical));
    }

    return result;
}

qlonglong Device::deviceId()
{
    //TODO determinar el numero de serie por comandos AT

    qlonglong serialNumber(1);
    return serialNumber;
}

NumberList Device::phoneNumbers() const
{
    return _numbers;
}

void Device::messageReceived(const QString &frame)
{
    qDebug("Incomming frame: %s", qPrintable(frame));

    PhoneNumber *number = (PhoneNumber *)_numbers.first();
    AbstractRecord *newRecord = _message.disassemble(frame);

    if (!newRecord)
        return;

    if (newRecord->type() == recInconmingMessage)
    {
        IncomingSMSRecord *incomingSMS = (IncomingSMSRecord *)newRecord;
        Message *newSMS = new Message(normalizeNumber(incomingSMS->from()),
                                      normalizeNumber(number->number()),
                                      incomingSMS->date(),
                                      incomingSMS->body(),
                                      MessageStatus::Idle);

        emit number->newMessageReceived((IMessage *)newSMS);
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

