#ifndef SMS_HPP
#define SMS_HPP

#include <QString>
#include <QDate>

namespace Gateway
{
    enum SMSStatus
    {
        stSent,
        stSending,
        stQueued
    };

    class SMS
    {
    public:
        SMS()
        {}

        SMS(const QString &from, const QString &to, const QDate &date,
            const QString &body, SMSStatus status)
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

        void setDate(const QDate &date)
        {
            _date = date;
        }

        QDate date() const
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

        void setStatus(SMSStatus status)
        {
            _status = status;
        }

        SMSStatus status() const
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
        QDate _date;
        QString _body;
        SMSStatus _status;
    };
}

#endif // SMS_HPP
