#include <include/CommonErrors.hpp>

#include "Device.hpp"
#include "PhoneNumber.hpp"
#include "Message.hpp"

#include <protocol/records/IncomingSMSRecord.hpp>

const ulong TimeOut = 3000;

Device::Device(const DeviceInfo &info)
{
    _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));

    _physical = new SerialPhysicalLayer(_serialPort);
    connect(_physical, SIGNAL(frameReceived(QString)), this, SLOT(messageReceived(QString)));
}

ulong Device::initialize()
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

    if (result)
    {
        _numbers.append(new PhoneNumber);
        return Error::OK;
    }

    return Error::errDeviceNotInitialized;
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

DeviceClass Device::type() const
{
    return devGSM;
}


void Device::messageReceived(const QString &frame)
{
    PhoneNumber *number = (PhoneNumber *)_numbers.first();

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
                                      incomingSMS->date(),
                                      incomingSMS->body(),
                                      MessageStatus::Idle);

        emit number->newMessageReceived((IMessage *)newSMS);

/////////
        bool result(_physical->send(_message.assemble(cmACK, CommandArgs())));
        if (result)
        {
            qDebug("Entro al if");
            QString data;
            result = _physical->receive(TimeOut, data);
             qDebug("--- Device response %s", qPrintable(data));
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

