#include "PhoneNumber.hpp"

PhoneNumber::PhoneNumber()
{
}

QString PhoneNumber::number() const
{
    return QString("5352501095");
}

void PhoneNumber::sendMessage(const IMessage *message)
{
    Q_UNUSED(message);
    //send message ...
}

MessageList PhoneNumber::unreadMessages() const
{
    return MessageList();
}

double PhoneNumber::balance() const
{
    return 0.00;
}
