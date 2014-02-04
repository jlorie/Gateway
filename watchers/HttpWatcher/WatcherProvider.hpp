#ifndef WACTHERPROVIDER_HPP
#define WACTHERPROVIDER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/WatcherInterface.hpp>
#include "HttpWatcher.hpp"

using namespace Gateway::Watcher;

class WactherProvider : public QObject, public WatcherInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Gateway.WatcherInterface")
    Q_INTERFACES(WatcherInterface)

    IWatcher *newWatcher(WatcherInfo *info)
    {
        return new HttpWatcher(info);
    }

    QString watcherName() const
    {
        return QString("HttpWatcher");
    }

    QString description() const
    {
        return QString("Watch for events on main server through HTTP protocol and notify to data server");
    }
};

#endif // WACTHERPROVIDER_HPP
