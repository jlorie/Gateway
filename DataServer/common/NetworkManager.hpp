#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <QObject>
#include <QMap>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QByteArray>

namespace Gateway
{
    typedef QMap<const QNetworkRequest *, QByteArray> PendingRequests;

    class NetworkManager: public QObject
    {
        Q_OBJECT
    public:
        explicit NetworkManager(QObject *parent = 0);

        virtual QNetworkReply *post(const QNetworkRequest &request, const QByteArray &postData);
        virtual QNetworkReply *get(const QNetworkRequest &request);

        bool waitForResponse();
    signals:
        void authenticationRequired (QNetworkReply * reply, QAuthenticator * authenticator);
        void finished (QNetworkReply * reply);

    private slots:
        void discardRequest(QNetworkReply *reply);

    private:
        QNetworkAccessManager _manager;
        PendingRequests _pendingRequests;
        QEventLoop _loop;

        uint _retries;
        QNetworkReply::NetworkError _networkError;
    };
}
#endif // NETWORKMANAGER_HPP
