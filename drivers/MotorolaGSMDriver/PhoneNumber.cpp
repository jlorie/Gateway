#include "PhoneNumber.hpp"
#include <protocol/ATMessage.hpp>

const int TimeOut = 5000;

PhoneNumber::PhoneNumber(SerialPhysicalLayer *physical)
    :_physical(physical)
{
}

QString PhoneNumber::number() const
{
    return QString("52501095");
}

void PhoneNumber::sendMessage(const IMessage *message)
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
    }}

MessageList PhoneNumber::unreadMessages() const
{
    return MessageList();
}
