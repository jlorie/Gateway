#ifndef WATCHER_HPP
#define WATCHER_HPP

#include <include/IWatcher.hpp>
#include <include/DataStructures/MessageList.hpp>
#include <include/DataStructures/MainInfo.hpp>

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
        explicit HttpWatcher();

        void start();
        void stop();

        MessageList pendingMessages();

    public slots:
        void handleResponse(QNetworkReply *reply);
        void authenticate(QNetworkReply *reply, QAuthenticator *authenticator);
        void poll();

    private:
        MessageList decodeFromJson(const QJsonArray &messageArray);

    private:
        QNetworkAccessManager _networkManager;
        MainInfo *_info;

        QTimer _pollingTimer;
        bool _waitingResponse;
    };
}
}

#endif // WATCHER_HPP
