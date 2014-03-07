#include "AMQPWatcherProvider.hpp"

namespace Gateway
{
namespace Watcher
{
    IWatcher *AMQPWactherProvider::newWatcher(const WatcherInfo &info)
    {
        return new AMQPWatcher(info);
    }

    QString AMQPWactherProvider::watcherName() const
    {
        return QString("AMQPWatcher");
    }

    QString AMQPWactherProvider::description() const
    {
        return QString("Watch for events on main server through AMQP protocol");
    }

    Q_EXPORT_PLUGIN2(amqpWatcher, AMQPWactherProvider)
}
}
