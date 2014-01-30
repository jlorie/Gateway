#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <include/IWatcher.hpp>
#include <include/DataStructures/WatcherInfo.hpp>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QTimer>

namespace Gateway
{
namespace Watcher
{

    class HttpWatcher : public IWatcher
    {
        Q_OBJECT
    public:
        explicit HttpWatcher(WatcherInfo *info);

    public slots:
        void handleResponse(QNetworkReply *reply);
        void authenticate(QNetworkReply *reply, QAuthenticator *authenticator);
        void poll();

    private:
        QNetworkAccessManager _networkManager;
        WatcherInfo *_info;

        QTimer _pollingTimer;
    };
}
}

#endif // WATCHER_HPP
