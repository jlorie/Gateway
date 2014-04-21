#include <RemoteStorage.hpp>

#include <SystemConfig.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>

#include <QByteArray>
#include <QEventLoop>
#include <QTimer>

#include <QVariantMap>
#include <qjson/parser.h>

#include <QDebug>

namespace Gateway
{
    const uint TimeOut = 1000;

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
            request.setUrl(QUrl(SystemConfig::instance()->value("http_url").toString() + "sms/"));
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

    void RemoteStorage::notifyMessageStatus(const IMessage *message, MessageStatus status)
    {
        qDebug(">> Changing message status %lld to %s", message->id(), (status == stSent ? "\"sent\"" : "\"failed\""));

        if (message->statusCallBack().isEmpty())
        {
            qWarning("Message with id %lld has not status callback", message->id());
        }

        QNetworkRequest request;
        {
            request.setUrl(QUrl(message->statusCallBack()));
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        }

        QByteArray postData;
        {
            postData.append(QByteArray("&status=")).append((status == stSent ? "sent" : "failed"));
        }

        _networkManager.post(request, postData);
    }

    MessageList RemoteStorage::pendingMessages()
    {
        MessageList result;

        QUrl urlRequest(SystemConfig::instance()->value("http_url").toString() + "sms/");
        {
            urlRequest.addQueryItem(QString("status"),QString("sending"));

            urlRequest.addQueryItem(QString("page"), QString::number(0));
            urlRequest.addQueryItem(QString("page_size"), QString::number(1000));
        }

        forever
        {
            _networkManager.get(QNetworkRequest(urlRequest));
            if (_networkManager.waitForResponse())
            {
                break;
            }
            else
            {
                QEventLoop loop;
                {
                    QTimer::singleShot(60 * 1000, &loop, SLOT(quit()));
                    loop.exec();
                }
            }
        }

        // Finally we get a response
        QJson::Parser parser;

        QVariantMap responseMap = parser.parse(_lastNetworkResponse).toMap();

        if (responseMap.contains("sms"))
        {
            QVariantList messageList(responseMap.value("sms").toList());
            foreach (QVariant value, messageList)
            {
                QVariantMap sms(value.toMap());
                {
                    qlonglong id = sms.value("id").toLongLong();
                    QString from = sms.value("from").toString();
                    QString to = sms.value("to").toString();
                    QString body = sms.value("body").toString();

                    QString statusCallback = sms.value("status_callback").toString();

                    result.append(new MessageInfo(from, to, body, id, statusCallback));
                }
            }
        }

        return result;
    }

    void RemoteStorage::requestFinished(QNetworkReply* reply)
    {
        if (!reply->error())
        {
            _lastNetworkResponse = reply->readAll();
            qDebug() << _lastNetworkResponse;
        }
    }


    void RemoteStorage::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
    {
        Q_UNUSED(reply);

        SystemConfig *config = SystemConfig::instance();
        authenticator->setUser(config->value("http_username").toString());
        authenticator->setPassword(config->value("http_password").toString());
    }

    RemoteStorage::RemoteStorage()
    {
        connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
        connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
                this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
    }

}
