#include "XmppWatcherProvider.hpp"
#include "XmppWatcher.hpp"

namespace Gateway
{
namespace Watcher
{
    IWatcher *XmppWatcherProvider::newWatcher(WatcherInfo *info)
    {
        return new XmppWatcher(info);
    }

    QString XmppWatcherProvider::watcherName() const
    {
        return QString("XmppWatcher");
    }

    QString XmppWatcherProvider::description() const
    {
        return QString("Watch for events on main server through Xmpp protocol and notify to data server");
    }

    Q_EXPORT_PLUGIN2(xmppWatcher, XmppWatcherProvider)
}
}
