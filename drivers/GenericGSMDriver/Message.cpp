#include "Message.hpp"

Message::Message()
{}

Message::Message(const QString &from, const QString &to, const QDateTime &date,
    const QString &body, MessageStatus status)
{
    _from = from;
    _to = to;
    _date = date;
    _body = body;
    _status = status;
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

void Message::setDate(const QDateTime &date)
{
    _date = date;
}

QDateTime Message::date() const
{
    return _date;
}

void Message::setBody(const QString &body)
{
    _body = body;
}

QString Message::body() const
{
    return _body;
}

void Message::setStatus(MessageStatus status)
{
    _status = status;
}

MessageStatus Message::status() const
{
    return _status;
}
