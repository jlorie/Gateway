#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <include/IMessage.hpp>

class Message: public IMessage
{
    public:
        Message();
        Message(const QString &from, const QString &to, const QString &body);

        qlonglong id() const
        {
            return 0;
        }

        void setFrom(const QString &from);
        QString from() const;

        void setTo(const QString &to);
        QString to() const;

        void setBody(const QString &body);
        QString body() const;

    private:
        QString _from;
        QString _to;
        QString _body;
};

#endif // SMS_HPP
