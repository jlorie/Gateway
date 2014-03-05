#include "HttpWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>
#include <SystemConfig.hpp>

#include <QUrl>
#include <QAuthenticator>
#include <QNetworkRequest>

#include <QTime>
#include <QByteArray>
#include <QEventLoop>
#include <qjson/parser.h>

#include <QDebug>

namespace Gateway
{
namespace Watcher
{
    HttpWatcher::HttpWatcher(const WatcherInfo &info)
        :_info(info)
    {
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
            _pollingTimer.setInterval(_info.value("http_poll_interval", "5").toInt() * 1000);
            _pollingTimer.start();
        }
    }

    void HttpWatcher::stop()
    {
        _pollingTimer.stop();
    }

    void HttpWatcher::poll()
    {
        // check time
        if (QTime::currentTime().hour() < 7)
            activateLowComsumption(true);
        else
            activateLowComsumption(false);

        QUrl urlRequest(_info.value("http_url") + "sms/");
        {
            urlRequest.addQueryItem(QString("status"),QString("sending"));
            urlRequest.addQueryItem(QString("page"), QString::number(0));
            urlRequest.addQueryItem(QString("page_size"), QString::number(1000));
        }

        _networkManager.get(QNetworkRequest(urlRequest));
    }

    void HttpWatcher::handleResponse(QNetworkReply *reply)
    {
        if (_waitingResponse)
            return;

        QByteArray response (reply->readAll());
        QJson::Parser parser;
        QVariantMap jsonResponse(parser.parse(response).toMap());

        if (jsonResponse.contains("sms"))
        {
            MessageList messages = decodeFromJson(jsonResponse.value("sms").toList());
            foreach (IMessage *message, messages)
            {
                emit messageReceived(message);
            }
        }
    }

    MessageList HttpWatcher::decodeFromJson(const QVariantList &messageList)
    {
        MessageList result;

        foreach (QVariant value, messageList)
        {
            QVariantMap sms(value.toMap());
            {
                qlonglong id = sms.value("id").toLongLong();
                QString from = sms.value("from").toString();
                QString to = sms.value("to").toString();
                QString body = sms.value("body").toString();

                result.append(new MessageInfo(from, to, body, id));
            }
        }

        return result;
    }

    void HttpWatcher::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
    {
        Q_UNUSED(reply);

        authenticator->setUser(_info.value("http_username", "guest"));
        authenticator->setPassword(_info.value("http_password", "guest"));
    }

    void HttpWatcher::activateLowComsumption(bool active)
    {
        if (active == _lowComsumtionActive)
            return;

        if (active)
        {
            qDebug(">> Activating low network comsumption");
            _pollingTimer.setInterval(60 * 5 * 1000); // 5 mins
        }
        else
        {
            qDebug(">> Deactivating low network comsumption");
            _pollingTimer.setInterval(_info.value("http_poll_interval", "5").toInt() * 1000);
        }

        _lowComsumtionActive = active;
    }
}
}
