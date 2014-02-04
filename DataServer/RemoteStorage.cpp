#include <RemoteStorage.hpp>

#include <SystemConfig.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QUrlQuery>

#include <QByteArray>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    void RemoteStorage::destroyInstance()
    {
        delete _instance;
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
        qDebug(">> Notifying message sent %s", (status == stSent ? "Sent" : "Failed"));
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

    MessageList RemoteStorage::pendingMessages()
    {
        MessageList result;

        QUrl urlRequest(_config->value("http_url") + "sms/");
        QUrlQuery query;
        {
            query.addQueryItem(QString("status"),QString("sending"));

            query.addQueryItem(QString("page"), QString::number(0));
            query.addQueryItem(QString("page_size"), QString::number(1000));
        }

        urlRequest.setQuery(query);

        QNetworkReply *reply = _networkManager.get(QNetworkRequest(urlRequest));
        QEventLoop loop;
        {
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));

            _waitingResponse = true;
            loop.exec();
            _waitingResponse = false;
        }

        if (!reply->error())
        {
            QByteArray response (reply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

            if (jsonResponse.object().contains("sms"))
            {
                QJsonArray messageArray(jsonResponse.object().value("sms").toArray());
                foreach (QJsonValue value, messageArray)
                {
                    QJsonObject object(value.toObject());
                    {
                        qlonglong id = object.value(QString("id")).toVariant().toInt();
                        QString from = object.value(QString("from")).toString();
                        QString to = object.value(QString("to")).toString();
                        QString body = object.value(QString("body")).toString();

                        result.append(new MessageInfo(from, to, body, id));
                    }
                }
            }
        }
        else
        {
            // TODO retry
        }

        return result;
    }

    void RemoteStorage::requestFinished(QNetworkReply* reply)
    {
        if (_waitingResponse)
            return;

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
        _waitingResponse = false;
        _config = SystemConfig::instance()->mainInfo();

        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
    }

}
