#ifndef IMESSAGE_HPP
#define IMESSAGE_HPP

#include <QString>
#include <QDate>
#include <QObject>

enum class MessageStatus
{
    Idle,
    Queued,
    Sending,
    Sent,
    Delivered
};

class IMessage: public QObject
{
    Q_OBJECT
    public:
        virtual QString from() const = 0;
        virtual QString to() const = 0;
        virtual QDate date() const = 0;

        virtual QString body() const = 0;
        virtual MessageStatus status() const = 0;

    signals:
        void stateChanged(MessageStatus state);
};


#endif // IMESSAGE_HPP
