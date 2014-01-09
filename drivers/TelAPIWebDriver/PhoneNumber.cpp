#include "PhoneNumber.hpp"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

PhoneNumber::PhoneNumber(NetworkManager *networkManager, const QString &number)
    :_networkManager(networkManager), _number(number)
{
    _networkManager->setResponseFormat(ResponseFormat::Json);

    connect(_networkManager, SIGNAL(responseReady(qlonglong,QByteArray)),
            this, SLOT(processResponse(qlonglong, QByteArray)));
}

PhoneNumber::~PhoneNumber()
{

}

QString PhoneNumber::number() const
{
    return _number;
}

void PhoneNumber::sendMessage(const IMessage *message)
{
    QByteArray postData;
    {
        postData.append(QByteArray("To=")).append(message->to());
        postData.append(QByteArray("&From=")).append(_number);
        postData.append(QByteArray("&Body=")).append(message->body());
    }
    QUrl query(_networkManager->baseRequestUrl().toString().append(QString("/SMS/Messages")));

    qlonglong requestId(_networkManager->post(query, postData));
    _pendingResponses.insert(requestId, RequestType::SendMessage);
    _pendingMessageResponse.insert(requestId, message);
}

MessageList PhoneNumber::unreadMessages() const
{
    return MessageList();
}

void PhoneNumber::processResponse(qlonglong requestId, QByteArray response)
{
    switch (_pendingResponses.take(requestId))
    {
        case RequestType::SendMessage:
        {
            processMessageRequest(_pendingMessageResponse.take(requestId), response);
            break;
        }
        case RequestType::UnreadMessages:
        {
            break;
        }
        default:
        {
            qWarning("Invalid response id: %lld", requestId);
            break;
        }
    };
}


void PhoneNumber::processMessageRequest(const IMessage *message, const QByteArray &response)
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
        if(jsonObject.value(QString("status")) == QString("queued"))
        {
            emit messageSent(message);
        }
    }
    else
    {
        qWarning("Error trying to send message");
    }

}
