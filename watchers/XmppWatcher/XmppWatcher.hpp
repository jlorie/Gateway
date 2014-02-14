#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <include/IWatcher.hpp>
#include <include/DataStructures/WatcherInfo.hpp>

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppMessage.h>

namespace Gateway
{
namespace Watcher
{
    class XmppWatcher : public IWatcher
    {
        Q_OBJECT
    public:
        explicit XmppWatcher(WatcherInfo *info);

        void start();
        void stop();

    public slots:
        void messageReceived(const QXmppMessage &message);
        void onConnect();

    private:
        WatcherInfo *_info;
        QXmppClient _xmppClient;

    };
}
}

#endif // WATCHER_HPP
