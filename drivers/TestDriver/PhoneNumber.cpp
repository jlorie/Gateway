#include "PhoneNumber.hpp"

PhoneNumber::PhoneNumber(const QString &number)
{
    Q_UNUSED(number);
}

PhoneNumber::~PhoneNumber()
{
}

QString PhoneNumber::number() const
{
    return QString("000000");
}

void PhoneNumber::sendMessage(const IMessage *message)
{
    Q_UNUSED(message;)
}

MessageList PhoneNumber::unreadMessages() const
{
    return MessageList();
}
