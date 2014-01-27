#include "Message.hpp"

Message::Message()
{}

Message::Message(const QString &from, const QString &to, const QString &body)
{
    _from = from;
    _to = to;
    _body = body;
}

void Message::setFrom(const QString &from)
{
    _from = from;
}

QString Message::from() const
{
    return _from;
}

void Message::setTo(const QString &to)
{
    _to = to;
}

QString Message::to() const
{
    return _to;
}

void Message::setBody(const QString &body)
{
    _body = body;
}

QString Message::body() const
{
    return _body;
}
