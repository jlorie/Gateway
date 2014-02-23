#include "NetworkManager.hpp"

#include <SystemConfig.hpp>
#include <QCryptographicHash>

#include <QTimer>
#include <QFile>
#include <QDir>

namespace Gateway
{
    const uint defaultRetries = 3;
    const uint defaultRetryTimeOut = 3000;

    NetworkManager::NetworkManager(QObject *parent)
        :QObject(parent)
    {
        connect(&_manager,SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)));
        connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
        connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(discardRequest(QNetworkReply*)));

        _retries = defaultRetries;
        _networkError = QNetworkReply::NoError;

        if (!_pendingRequests.isEmpty())
            retryFailedRequests();
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
        if (reply->error() >= 201 && reply->error() <= 299) //Content error
        {
            qWarning("Server error %s ... discarding request ...", qPrintable(reply->errorString()));
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
            qWarning("Could not stablish connection with main server... %s", qPrintable(reply->errorString()));

            if (_loop.isRunning())
                _loop.quit();

            _retries = defaultRetries;

            // Solo se guardan como pendientes las notifiaciones al servidor principal
            if (reply->operation() == QNetworkAccessManager::PostOperation)
                _pendingRequests.enqueue(reply->request());

            QTimer::singleShot(60 * defaultRetryTimeOut, this, SLOT(retryFailedRequests()));
        }

        _networkError = reply->error();
    }

    void NetworkManager::retryFailedRequests()
    {
        if (_pendingRequests.isEmpty())
            return;

        qDebug("~~~~> Retrying failed requests ...");

        while (!_pendingRequests.isEmpty())
        {
            QNetworkRequest request(_pendingRequests.dequeue());
            QByteArray postData(request
                                .attribute((QNetworkRequest::Attribute)AttrPostData)
                                .toByteArray());

            this->post(request, postData);
        }
    }
}
