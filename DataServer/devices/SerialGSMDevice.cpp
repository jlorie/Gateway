#include "SerialGSMDevice.hpp"
#include "protocol/records/IncomingSMSRecord.hpp"

#include <typeinfo>
#include <QDebug>


namespace Gateway
{
    const ulong TimeOut = 1000;

    SerialGSMDevice::SerialGSMDevice(const DeviceInfo &info, QObject *parent)
        :AbstractGSMDevice(info, parent)
    {

    }

    bool SerialGSMDevice::initialize()
    {
        bool result (false);

        _physical = new SerialPhysicalLayer(QSerialPortInfo(_deviceInfo.name));
        connect(_physical, SIGNAL(frameReceived(QString)), this, SLOT(messageReceived(QString)));

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

    void SerialGSMDevice::messageReceived(const QString &frame)
    {
        qDebug("Message Received");
        AbstractRecord *newRecord = _message.disassemble(frame);

        if (!newRecord)
            return;

        if (newRecord->type() == recInconmingMessage)
        {
            IncomingSMSRecord *incomingSMS = (IncomingSMSRecord *)newRecord;
            SMS newSMS(incomingSMS->from(), QString("this"), incomingSMS->date(),
                       incomingSMS->body(), stQueued);

            emit newSMSReceived(newSMS);
        }
    }

}
