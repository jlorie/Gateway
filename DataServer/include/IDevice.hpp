#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include "IMessage.hpp"

#include <QList>
#include <QObject>

class IDevice : public QObject
{
Q_OBJECT
public:
    virtual ~IDevice() {}

    virtual QString deviceId() = 0;
    virtual void sendMessage(const IMessage *message) = 0;

signals:
    void newMessageReceived(const IMessage *message);
    void messageStatusChanged(qlonglong messageId, MessageStatus status);
    void connectionClosed();
};

#endif // IDEVICE_HPP
