#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <include/IMessage.hpp>

class Message: public IMessage
{
    public:
        Message();
        Message(const QString &from, const QString &to, const QDate &date, const QString &body, MessageStatus status);

        void setFrom(const QString &from);
        QString from() const;

        void setTo(const QString &to);
        QString to() const;

        void setDate(const QDate &date);
        QDate date() const;

        void setBody(const QString &body);
        QString body() const;

        void setStatus(MessageStatus status);
        MessageStatus status() const;

    private:
        QString _from;
        QString _to;
        QDate _date;
        QString _body;
        MessageStatus _status;
};

#endif // SMS_HPP