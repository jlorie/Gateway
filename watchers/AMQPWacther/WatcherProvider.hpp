#ifndef WACTHERPROVIDER_HPP
#define WACTHERPROVIDER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/WatcherInterface.hpp>
#include "AMQPWatcher.hpp"

using namespace Gateway::Watcher;

class WactherProvider : public QObject, public WatcherInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Gateway.WatcherInterface")
    Q_INTERFACES(WatcherInterface)

    IWatcher *newWatcher(const WatcherInfo &info)
    {
        return new AMQPWatcher(info);
    }

    QString watcherName() const
    {
        return QString("AMQPWatcher");
    }

    QString description() const
    {
        return QString("Watch for events on main server through AMQP protocol and notify to data server");
    }
};

#endif // WACTHERPROVIDER_HPP
