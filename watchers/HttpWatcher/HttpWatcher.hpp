#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <include/IWatcher.hpp>
#include <include/DataStructures/MessageList.hpp>
#include <include/DataStructures/WatcherInfo.hpp>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QVariantList>
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

        void start();
        void stop();

    public slots:
        void handleResponse(QNetworkReply *reply);
        void authenticate(QNetworkReply *reply, QAuthenticator *authenticator);
        void poll();

    private:
        MessageList decodeFromJson(const QVariantList &messageList);
        void activateLowComsumption(bool active);

    private:
        QNetworkAccessManager _networkManager;
        WatcherInfo *_info;

        QTimer _pollingTimer;
        bool _waitingResponse;
        bool _lowComsumtionActive;
    };
}
}

#endif // WATCHER_HPP
