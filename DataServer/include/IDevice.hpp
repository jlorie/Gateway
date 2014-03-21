#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include "IMessage.hpp"

#include <QList>
#include <QObject>

typedef QList<IMessage *> MessageList;

class IDevice : public QObject
{
Q_OBJECT
public:
    virtual ~IDevice() {}

    virtual QString deviceIMEI() = 0;
    virtual void sendMessage(const IMessage *message) = 0;
    virtual MessageList pendingMessages() const = 0;

signals:
    void newMessageReceived(const IMessage *message);
    void messageStatusChanged(qlonglong messageId, MessageStatus status);
    void connectionClosed();
};

#endif // IDEVICE_HPP
