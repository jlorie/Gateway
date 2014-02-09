#include "HttpWatcherProvider.hpp"
#include "HttpWatcher.hpp"

namespace Gateway
{
namespace Watcher
{
    IWatcher *HttpWatcherProvider::newWatcher(WatcherInfo *info)
    {
        return new HttpWatcher(info);
    }

    QString HttpWatcherProvider::watcherName() const
    {
        return QString("HttpWatcher");
    }

    QString HttpWatcherProvider::description() const
    {
        return QString("Watch for events on main server through HTTP protocol and notify to data server");
    }

    Q_EXPORT_PLUGIN2(httpWatcher, HttpWatcherProvider)
}
}
