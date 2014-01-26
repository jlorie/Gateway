#ifndef AMQPWATCHER_HPP
#define AMQPWATCHER_HPP

#include "RemoteWatcher.hpp"
#include <qamqp/amqp.h>
#include <qamqp/amqp_queue.h>

namespace Gateway
{
    class AMQPWatcher : public RemoteWatcher
    {
        Q_OBJECT
    public:
        explicit AMQPWatcher(QObject *parent = 0);

    protected slots:
        void declared();
        void newMessage(QAMQP::Queue * q);

    private:
        QAMQP::Queue *_queue;
        QAMQP::Client *_client;
    };
}
#endif // AMQPWATCHER_HPP
