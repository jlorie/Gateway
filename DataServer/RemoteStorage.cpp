#include <RemoteStorage.hpp>

#include <SystemConfig.hpp>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QAuthenticator>

#include <QDebug>

namespace Gateway
{

    const uint TimeOut = 5000;

    RemoteStorage *RemoteStorage::_instance = 0;

    bool RemoteStorage::initialize()
    {
        if (!_instance)
        {
            _instance = new RemoteStorage;
            qDebug(">> Remote storage initialized!!!");
        }

        return true;
    }

    RemoteStorage *RemoteStorage::instance()
    {
        return _instance;
    }

    void RemoteStorage::dispatchMessage(const IMessage *message)
    {
        qDebug(">> Dispatching message");
        qDebug("----> from: %s", qPrintable(message->from()));
        qDebug("----> to: %s", qPrintable(message->to()));
        qDebug("----> body: %s", qPrintable(message->body()));

        QNetworkRequest request;
        {
            request.setUrl(QUrl(_config->value("http_url") + "sms/"));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        }

        QByteArray postData;
        {
            postData.append(QByteArray("to=")).append(message->to());
            postData.append(QByteArray("&from=")).append(message->from());
            postData.append(QByteArray("&body=")).append(message->body());
        }

        _networkManager.post(request, postData);

    }

    void RemoteStorage::notifyMessageStatus(qlonglong messageId, MessageStatus status)
    {
        qDebug(">> Notifying message sent");
        qDebug("----> id: %lld", messageId);

        QString username(_config->value("http_username"));
        QString password(_config->value("http_password"));

        QNetworkRequest request;
        {
            request.setUrl(QUrl(QString(_config->value("http_url") + "sms/status/%1/%2")
                                .arg(username, password)));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        }

        QByteArray postData;
        {
            postData.append(QByteArray("id=")).append(QString::number(messageId));
            postData.append(QByteArray("&status=")).append((status == stSent ? "sent" : "failed"));
        }

        _networkManager.post(request, postData);
    }

    void RemoteStorage::requestFinished(QNetworkReply* reply)
    {
        if (reply->error())
        {
            qWarning("Request error: %s", qPrintable(reply->errorString()));
            return;
        }

        qDebug() << reply->readAll();
    }


    void RemoteStorage::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
    {
        Q_UNUSED(reply);

        authenticator->setUser(_config->value("http_username"));
        authenticator->setPassword(_config->value("http_password"));
    }


    RemoteStorage::RemoteStorage()
    {
        _config = SystemConfig::instance()->mainInfo();

        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
    }

}
