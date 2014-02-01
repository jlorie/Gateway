#ifndef IWATCHER_HPP
#define IWATCHER_HPP

#include "IMessage.hpp"

#include <QObject>

class IWatcher : public QObject
{
    Q_OBJECT
public:
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void messageReceived(const IMessage *message);
};


#endif // IWATCHER_HPP
