#include "NetworkManager.hpp"

namespace Gateway
{

    NetworkManager::NetworkManager(QObject *parent) :
        QNetworkAccessManager(parent)
    {
        connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(discardRequest(QNetworkReply *)));
    }

    void NetworkManager::retry(const QNetworkRequest &request)
    {
        qlonglong requestId = qlonglong(&request);
        if (_pendingRequests.contains(requestId))
        {
            RequestInfo *info(_pendingRequests.take(requestId));
            createRequest(info->operation(), request, info->outgoingData());

            delete info;
        }
    }

    QNetworkReply *NetworkManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
    {
        _pendingRequests.insert(qlonglong(&request), new RequestInfo(op, outgoingData));
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }

    void NetworkManager::discardRequest(QNetworkReply *reply)
    {
        if (!reply->error())
        {
            const QNetworkRequest &request(reply->request());
            RequestInfo *info(_pendingRequests.take(qlonglong(&request)));

            delete info;
        }
    }

}
