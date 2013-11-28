#ifndef IPHONE_HPP
#define IPHONE_HPP

#include <common/SMS.hpp>

#include <QObject>
#include <QString>
#include <QList>

namespace Gateway
{
    typedef QList<SMS *> SMSList;
    class IPhone
    {
        Q_OBJECT
    public:
        virtual QString phoneNumber() const = 0;
        virtual void sendMessage(const SMS *message) = 0;
        virtual SMSList unreadMessages() const = 0;

    signals:
        newMessageReceived(const SMS *message);
    };
}

#endif // IGSMDEVICE_HPP
