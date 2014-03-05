#ifndef WACTHERPROVIDER_HPP
#define WACTHERPROVIDER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/WatcherInterface.hpp>
#include "HttpWatcher.hpp"

namespace Gateway
{
namespace Watcher
{
    class HttpWatcherProvider : public QObject, public WatcherInterface
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
