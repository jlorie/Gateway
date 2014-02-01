#include "HttpWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>
#include <SystemConfig.hpp>

#include <QUrl>
#include <QUrlQuery>
#include <QAuthenticator>
#include <QNetworkRequest>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QEventLoop>

#include <QDebug>

namespace Gateway
{
namespace Watcher
{
    HttpWatcher::HttpWatcher()
    {
        _info = SystemConfig::instance()->mainInfo();
        _waitingResponse = false;

        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleResponse(QNetworkReply*)));

        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));

        connect(&_pollingTimer, SIGNAL(timeout()), this, SLOT(poll()));
    }

    void HttpWatcher::start()
    {
        poll(); //polling
        {
            _pollingTimer.setInterval(_info->value("http_poll_interval", "5").toInt() * 1000);
            _pollingTimer.start();
        }
    }

    void HttpWatcher::stop()
    {
        _pollingTimer.stop();
    }

    MessageList HttpWatcher::pendingMessages()
    {
        MessageList result;

        QUrl urlRequest(_info->value("http_url") + "sms/");
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
            connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
            _waitingResponse = true;
            loop.exec();
            _waitingResponse = false;
        }

        QByteArray response (reply->readAll());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

        if (jsonResponse.object().contains("sms"))
            result = decodeFromJson(jsonResponse.object().value(QString("sms")).toArray());

        return result;
    }

    void HttpWatcher::poll()
    {
        QUrl urlRequest(_info->value("http_url"));
        QUrlQuery query;
        {
            query.addQueryItem(QString("id_from"),
                               QString::number(_info->value("sms_last_id").toInt() + 1));

            query.addQueryItem(QString("status"),QString("sending"));
            query.addQueryItem(QString("page"), QString::number(0));
            query.addQueryItem(QString("page_size"), QString::number(1000));
        }

        urlRequest.setQuery(query);

        _networkManager.get(QNetworkRequest(urlRequest));
    }

    void HttpWatcher::handleResponse(QNetworkReply *reply)
    {
        if (_waitingResponse)
            return;

        QByteArray response (reply->readAll());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

        if (jsonResponse.object().contains("sms"))
        {
            MessageList messages = decodeFromJson(jsonResponse.object().value(QString("sms")).toArray());

            foreach (IMessage *message, messages)
            {
                emit messageReceived(message);
            }
        }
    }

    MessageList HttpWatcher::decodeFromJson(const QJsonArray &messageArray)
    {
        MessageList result;

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

        return result;
    }

    void HttpWatcher::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
    {
        Q_UNUSED(reply);

        authenticator->setUser(_info->value("http_username", "guest"));
        authenticator->setPassword(_info->value("http_password", "guest"));
    }
}
}
