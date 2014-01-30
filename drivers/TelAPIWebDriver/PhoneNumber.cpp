#include "PhoneNumber.hpp"

#include <QNetworkRequest>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QList>

PhoneNumber::PhoneNumber(NetworkManager *networkManager, const QString &number)
    :_networkManager(networkManager), _number(number)
{
    _networkManager->setResponseFormat(ResponseFormat::Json);

    connect(_networkManager, SIGNAL(responseReady(qlonglong,QByteArray)),
            this, SLOT(processResponse(qlonglong, QByteArray)));

    _smsListPageSize = 1;
    _configuring = true;
    _lastMessage = 0;

    connect(&_pollingTimer, SIGNAL(timeout()), this, SLOT(poll()));

    poll();
    _pollingTimer.setInterval(10000);
    _pollingTimer.start();
}

PhoneNumber::~PhoneNumber()
{

}

QString PhoneNumber::number() const
{
    return _number;
}

void PhoneNumber::poll()
{
    //Request for new messages
    unreadMessages();  
}

void PhoneNumber::sendMessage(const IMessage *message)
{
    QByteArray postData;
    {
        postData.append(QByteArray("To=")).append(message->to());
        postData.append(QByteArray("&From=")).append(_number);
        postData.append(QByteArray("&Body=")).append(message->body());
    }
    QUrl urlRequest(_networkManager->baseRequestUrl().toString().append(QString("/SMS/Messages")));

    qlonglong requestId(_networkManager->post(urlRequest, postData));
    _pendingResponses.insert(requestId, RequestType::SendMessage);
    _pendingMessageResponse.insert(requestId, message);
}

void PhoneNumber::unreadMessages()
{
    QUrl urlRequest(_networkManager->baseRequestUrl().toString().append(QString("/SMS/Messages")));

    QUrlQuery query;
    {
        query.addQueryItem(QString("Page"), QString::number(0));
        query.addQueryItem(QString("PageSize"), QString::number(_lastMessage ? 100: 1));

        if (_lastMessage)
        {
            QDateTime afterDate(_lastMessage->date().addSecs(1));
            query.addQueryItem(QString("DateSent"), afterDate.toString(QString("yyyy-MM-dd-hh:mm:ss")));
        }
    }

    urlRequest.setQuery(query);

    qlonglong requestId(_networkManager->get(urlRequest));
    _pendingResponses.insert(requestId, RequestType::UnreadMessages);
}

void PhoneNumber::processResponse(qlonglong requestId, QByteArray response)
{
    switch (_pendingResponses.take(requestId))
    {
        case RequestType::SendMessage:
        {
            processSendMessageRequest(_pendingMessageResponse.take(requestId), response);
            break;
        }
        case RequestType::UnreadMessages:
        {
            processUnreadMessageRequest(response);
            break;
        }
        default:
        {
            qWarning("Invalid response id: %lld", requestId);
            break;
        }
    };
}


void PhoneNumber::processSendMessageRequest(const IMessage *message, const QByteArray &response)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

    if (!jsonResponse.isObject())
    {
        qWarning("Empty reply!!!");
        return;
    }

    QJsonObject jsonObject(jsonResponse.object());

    if (jsonObject.find(QString("status")) != jsonObject.end())
    {
        if(jsonObject.value(QString("status")).toString() == QString("queued"))
        {
            emit messageSent(message);
        }
    }
    else
    {
        qWarning("Error trying to send message");
    }

}

void PhoneNumber::processUnreadMessageRequest(const QByteArray &response)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);

    if (!jsonResponse.isObject())
    {
        qWarning("Empty reply!!!");
        return;
    }

    QJsonArray smsList(jsonResponse.object().value(QString("sms_messages")).toArray());

    QList<IMessage *> messages;

    foreach (QJsonValue value, smsList)
    {
        QJsonObject object(value.toObject());
        {
            QString direction = object.value(QString("direction")).toString();
            if (direction != QString("inbound"))
                continue;

            QString from = object.value(QString("from")).toString();
            QString to = object.value(QString("to")).toString();
            QString body = object.value(QString("body")).toString();

            QString strDate = object.value(QString("date_created")).toString();
            QDateTime date = QDateTime::fromString(strDate, QString("ddd, d MMM yyyy hh:mm:ss +0000"));

            if (!date.isValid())
            {
                qWarning("Wrong date time format fetched from TelAPI");
                continue;
            }

            messages.prepend(new Message(from, to, date, body, MessageStatus::Idle));
        }
    }

    foreach (IMessage *message, messages)
    {
        _lastMessage = message;
        if (!_configuring)
        {
            emit newMessageReceived(message);
        }
    }

    _configuring = false;
}
