#include "HttpWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QUrl>
#include <QUrlQuery>
#include <QAuthenticator>
#include <QNetworkRequest>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

namespace Gateway
{
namespace Watcher
{
    HttpWatcher::HttpWatcher(WatcherInfo *info)
        :_info(info)
    {
        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(handleResponse(QNetworkReply*)));

        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));

        connect(&_pollingTimer, SIGNAL(timeout()), this, SLOT(poll()));

        poll(); //polling
        {
            _pollingTimer.setInterval(info->value("http_poll_interval", "5").toInt() * 1000);
            _pollingTimer.start();
        }
    }

    void HttpWatcher::poll()
    {
        QUrl urlRequest(_info->value("http_url"));
        QUrlQuery query;
        {
            query.addQueryItem(QString("id_from"),
                               QString::number(_info->value("sms_last_id").toInt() + 1));

            query.addQueryItem(QString("page"), QString::number(0));
            query.addQueryItem(QString("page_size"), QString::number(1000));
        }

        urlRequest.setQuery(query);

        _networkManager.get(QNetworkRequest(urlRequest));
    }

    void HttpWatcher::handleResponse(QNetworkReply *reply)
    {
        QByteArray response (reply->readAll());
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

        if (!jsonResponse.isObject())
        {
            qWarning("Empty reply!!!");
            return;
        }

        QJsonArray smsList(jsonResponse.object().value(QString("sms")).toArray());

        foreach (QJsonValue value, smsList)
        {
            QJsonObject object(value.toObject());
            {
                qlonglong id = object.value(QString("id")).toInt();
                QString from = object.value(QString("from")).toString();
                QString to = object.value(QString("to")).toString();
                QString body = object.value(QString("body")).toString();

                emit messageReceived(new MessageInfo(from, to, body, id));
                _info->insert("sms_last_id", QString::number(id));

            }
        }
    }

    void HttpWatcher::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
    {
        Q_UNUSED(reply);

        authenticator->setUser(_info->value("http_username", "guest"));
        authenticator->setPassword(_info->value("http_password", "guest"));
    }
}
}
