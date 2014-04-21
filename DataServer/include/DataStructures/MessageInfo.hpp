#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <include/IMessage.hpp>

class MessageInfo: public IMessage
{
    public:
        MessageInfo()
            :_id(0)
        {}

        MessageInfo(const QString &from, const QString &to, const QString &body,
                    const qlonglong id = 0, const QString statusCallBack = "")
            :_from(from), _to(to), _body(body), _id(id), _statusCallback(statusCallBack)
        {}

        void setId(const qlonglong id)
        {
            if (id != 0)
            {
                qWarning("Error setting up id, this message has already an id");
                return;
            }

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

        void setStatusCallback(const QString &statusCallBack)
        {
            _statusCallback = statusCallBack;
        }

        QString statusCallBack() const
        {
            return _statusCallback;
        }

    private:
        QString _from;
        QString _to;
        QString _body;
        qlonglong _id;

        QString _statusCallback;
};

#endif // SMS_HPP
