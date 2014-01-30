#ifndef IWATCHER_HPP
#define IWATCHER_HPP

#include "IMessage.hpp"

#include <QObject>

class IWatcher : public QObject
{
    Q_OBJECT
signals:
    void messageReceived(const IMessage *message);
    void messageSent(qlonglong messageId);
    void messageRefused(qlonglong messageId);
};


#endif // IWATCHER_HPP
