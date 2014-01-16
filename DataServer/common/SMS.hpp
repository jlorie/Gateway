#ifndef SMS_HPP
#define SMS_HPP

#include <include/IMessage.hpp>

#include <QString>
#include <QDate>

namespace Gateway
{
    class SMS : public IMessage
    {
    public:
        SMS()
        {}

        SMS(const QString &from, const QString &to, const QDateTime &date,
            const QString &body, MessageStatus status)
        {
            _from = from;
            _to = to;
            _date = date;
            _body = body;
            _status = status;
        }

        void setFrom(const QString &from)
        {
            _from = from;
        }

        QString from() const
        {
            return _from;
        }

        void setTo(const QString &to)
        {
            _to = to;
        }

        QString to() const
        {
            return _to;
        }

        void setDate(const QDateTime &date)
        {
            _date = date;
        }

        QDateTime date() const
        {
            return _date;
        }

        void setBody(const QString &body)
        {
            _body = body;
        }

        QString body() const
        {
            return _body;
        }

        void setStatus(MessageStatus status)
        {
            _status = status;
        }

        MessageStatus status() const
        {
            return _status;
        }

        QString toString() const
        {
            return QString("From: %1\n Body: %2").arg(_from).arg(_body);
        }

    private:

        QString _from;
        QString _to;
        QDateTime _date;
        QString _body;
        MessageStatus _status;
    };
}

#endif // SMS_HPP
