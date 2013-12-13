#ifndef IPHONENUMBER
#define IPHONENUMBER

#include "IMessage.hpp"

#include <QObject>
#include <QString>
#include <QList>

typedef QList<IMessage *> MessageList;

class IPhoneNumber: public QObject
{
    Q_OBJECT
public:
    virtual ~IPhoneNumber() {}

    virtual QString number() const = 0;
    virtual void sendMessage(const IMessage *message) = 0;
    virtual MessageList unreadMessages() const = 0;

    virtual double balance() const = 0;

    signals:
        void newMessageReceived(const IMessage *message);
};


#endif // IPHONENUMBER_HPP