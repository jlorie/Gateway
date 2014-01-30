#ifndef IWATCHER_HPP
#define IWATCHER_HPP

#include "IMessage.hpp"

#include <QObject>

class IWatcher : public QObject
{
    Q_OBJECT
signals:
    void messageReceived(const IMessage *message);
};


#endif // IWATCHER_HPP
