#ifndef SMS_HPP
#define SMS_HPP

#include <include/IMessage.hpp>

#include <QString>

namespace Gateway
{
    class SMS : public IMessage
    {
    public:
        SMS()
        {}

        SMS(const QString &from, const QString &to, const QString &body)
        {
            _from = from;
            _to = to;
            _body = body;
            _id = 0;
        }

        void setId(qlonglong id)
        {
            _id = id;
        }

        qlonglong id() const
        {
            return _id;
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

        void setBody(const QString &body)
        {
            _body = body;
        }

        QString body() const
        {
            return _body;
        }

        QString toString() const
        {
            return QString("From: %1\n Body: %2").arg(_from).arg(_body);
        }

    private:
        qlonglong _id;
        QString _from;
        QString _to;
        QString _body;
    };
}

#endif // SMS_HPP
