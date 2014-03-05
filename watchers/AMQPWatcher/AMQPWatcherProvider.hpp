#ifndef AMQPWACTHERPROVIDER_HPP
#define AMQPWACTHERPROVIDER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/WatcherInterface.hpp>
#include "AMQPWatcher.hpp"

namespace Gateway
{
namespace Watcher
{
    class AMQPWactherProvider : public QObject, public WatcherInterface
    {
        Q_OBJECT
        Q_INTERFACES(WatcherInterface)

        IWatcher *newWatcher(const WatcherInfo &info);

        QString watcherName() const;
        QString description() const;
    };
}
}
#endif // WACTHERPROVIDER_HPP
