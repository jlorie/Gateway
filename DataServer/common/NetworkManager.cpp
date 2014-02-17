#include "NetworkManager.hpp"

#include <QTimer>

namespace Gateway
{
    const uint defaultRetries = 3;
    const uint defaultRetryTimeOut = 300;

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
        QNetworkRequest withData = request;
        withData.setAttribute((QNetworkRequest::Attribute)AttrPostData, postData);

        return _manager.post(withData, postData);
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
            if (_loop.isRunning())
                _loop.quit();

            _retries = defaultRetries;
        }
        else
        if (--_retries)
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
                    QByteArray postData(reply->request()
                                        .attribute((QNetworkRequest::Attribute)AttrPostData)
                                        .toByteArray());

                    this->post(reply->request(), postData);
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            qWarning("Request error %s ... retrying ...", qPrintable(reply->errorString()));
            qWarning("Could not stablish connection with main server");
            QTimer::singleShot(60 * defaultRetryTimeOut, this, SLOT(retryFailedRequests()));

            if (_loop.isRunning())
                _loop.quit();

            _retries = defaultRetries;

            // Solo se guardan como pendientes las notifiaciones al servidor principal
            if (reply->operation() == QNetworkAccessManager::PostOperation)
                _pendingRequests.append(reply->request());
        }

        _networkError = reply->error();
    }

    void NetworkManager::retryFailedRequests()
    {
        qDebug("~~~~> Retrying failed requests ...");

        while (!_pendingRequests.isEmpty())
        {
            QNetworkRequest request(_pendingRequests.takeFirst());
            QByteArray postData(request
                                .attribute((QNetworkRequest::Attribute)AttrPostData)
                                .toByteArray());

            this->post(request, postData);
        }
    }

}
