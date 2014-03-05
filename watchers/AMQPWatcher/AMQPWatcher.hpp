#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <include/IWatcher.hpp>
#include <include/DataStructures/WatcherInfo.hpp>

#include <qamqp/amqp.h>
#include <qamqp/amqp_queue.h>

namespace Gateway
{
namespace Watcher
{

    class AMQPWatcher : public IWatcher
    {
        Q_OBJECT
    public:
        explicit AMQPWatcher(const WatcherInfo &info);

        void start();
        void stop();

    protected slots:
        void declared();
        void newMessage(QAMQP::Queue * q);

    private:
        QAMQP::Queue *_queue;
        QAMQP::Client *_client;

    };
}
}

#endif // WATCHER_HPP
