#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include "RequestInfo.hpp"

#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace Gateway
{
    typedef QMap<const QNetworkRequest *, RequestInfo *> PendingRequests;

    class NetworkManager : public QNetworkAccessManager
    {
        Q_OBJECT
    public:
        explicit NetworkManager(QObject *parent = 0);

        void retry(const QNetworkRequest *request);

    protected:
        virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

    private:
        PendingRequests _pendingRequests;
    };
}
#endif // NETWORKMANAGER_HPP
