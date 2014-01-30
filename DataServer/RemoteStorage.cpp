#include <RemoteStorage.hpp>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QAuthenticator>

#include <QDebug>

namespace Gateway
{

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
            request.setUrl(QUrl("http://cubania.info/app.php/api/sms/"));
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

        QString username("dfvshlxn");
        QString password("OZE08m61Q_QcN01owJSr4z0eo5cM-OUr");

        QNetworkRequest request;
        {
            request.setUrl(QUrl(QString("http://cubania.info/app.php/api/sms/status/%1/%2").arg(username, password)));
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

        authenticator->setUser(_username);
        authenticator->setPassword(_password);
    }


    RemoteStorage::RemoteStorage()
    {
        _username = QString("dfvshlxn");
        _password = QString("OZE08m61Q_QcN01owJSr4z0eo5cM-OUr");

        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
    }

}
