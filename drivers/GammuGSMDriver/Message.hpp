#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <include/IMessage.hpp>

class Message: public IMessage
{
    public:
        Message();

        QString from() const;
        QString to() const;
        QDateTime date() const;
        QString body() const;
        MessageStatus status() const;
};

#endif // SMS_HPP
