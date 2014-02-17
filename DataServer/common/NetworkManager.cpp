#include "NetworkManager.hpp"

namespace Gateway
{
    const uint defaultRetries = 3;

    NetworkManager::NetworkManager(QObject *parent)
        :QObject(parent)
    {
        connect(&_manager,SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
        connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(discardRequest(QNetworkReply*)));

        _retries = defaultRetries;
        _networkError = QNetworkReply::NoError;
    }

    QNetworkReply *NetworkManager::post(const QNetworkRequest &request, const QByteArray &postData)
    {
        _pendingRequests.insert(&request, postData);
        return _manager.post(request, postData);
    }

    QNetworkReply *NetworkManager::get(const QNetworkRequest &request)
    {
        return _manager.get(request);
    }

    bool NetworkManager::waitForResponse()
    {
        _loop.exec();

        return (_networkError == QNetworkReply::NoError);
    }

    void NetworkManager::discardRequest(QNetworkReply *reply)
    {
        if (!reply->error())
        {
            const QNetworkRequest &request(reply->request());
            _pendingRequests.take(&request);

            if (_loop.isRunning())
                _loop.quit();

            _retries = defaultRetries;
        }
        else
        if (_retries --)
        {
            qWarning("Request error %s ... retrying ...", qPrintable(reply->errorString()));

            switch (reply->operation())
            {
                case QNetworkAccessManager::GetOperation:
                {
                    this->get(reply->request());
                    break;
                }
                case QNetworkAccessManager::PostOperation:
                {
                    const QNetworkRequest &request = reply->request();
                    this->post(reply->request(), _pendingRequests.take(&request));
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            qWarning("Could not stablish connection with main server");
            qDebug("----> Saving pending requests ...");
            {
                //TODO guardar peticiones pendientes
            }

            if (_loop.isRunning())
                _loop.quit();

            _retries = defaultRetries;
        }

        _networkError = reply->error();
    }

}
