#ifndef IMESSAGE_HPP
#define IMESSAGE_HPP

#include <QString>
#include <QDate>

enum MessageStatus
{
    stIdle,
    stQueued,
    stSending,
    stSent,
    stDelivered
};

class IMessage
{
    public:

    virtual QString from() const = 0;

    virtual QString to() const = 0;

    virtual QDate date() const = 0;

    virtual QString body() const = 0;

    virtual MessageStatus status() const = 0;

};


#endif // IMESSAGE_HPP
