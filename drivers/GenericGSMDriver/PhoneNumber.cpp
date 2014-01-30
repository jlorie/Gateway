#include "PhoneNumber.hpp"

#include <protocol/ATMessage.hpp>
#include <QProcess>

const char CZ = 0x1A;
const char CR = 0x0D;

PhoneNumber::PhoneNumber(SerialPhysicalLayer *physical)
    :_physical(physical)
{
}

QString PhoneNumber::number() const
{
    return QString("+584120884437");
}

void PhoneNumber::sendMessage(const IMessage *message)
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

MessageList PhoneNumber::unreadMessages() const
{
    return MessageList();
}
