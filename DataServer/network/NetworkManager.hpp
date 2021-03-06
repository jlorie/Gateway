#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include "PendingRequestList.hpp"

#include <QObject>
#include <QMap>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>

namespace Gateway
{
    class NetworkManager: public QObject
    {
        Q_OBJECT
    public:
        explicit NetworkManager(QObject *parent = 0);

        virtual QNetworkReply *post(const QNetworkRequest &request, const QByteArray &postData);
        virtual QNetworkReply *get(const QNetworkRequest &request);

        bool waitForResponse();

        enum RequestAtribute
        {
            AttrPostData = 1001
        };
    signals:
        void authenticationRequired (QNetworkReply * reply, QAuthenticator * authenticator);
        void finished (QNetworkReply * reply);

    private slots:
        void discardRequest(QNetworkReply *reply);
        void retryFailedRequests();

    private:
        QNetworkAccessManager _manager;
        PendingRequestList _pendingRequests;
        QEventLoop _loop;

        uint _retries;
        QNetworkReply::NetworkError _networkError;
    };

}
#endif // NETWORKMANAGER_HPP
