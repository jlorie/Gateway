#include "AMQPWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

namespace Gateway
{
namespace Watcher
{
    using QAMQP::Queue;

    AMQPWatcher::AMQPWatcher(WatcherInfo *info)
    {
        _client = new QAMQP::Client(this);
        {
            _client->setHost(info->value("amqp_host", "localhost"));
            _client->setPort(info->value("amqp_port", "5672").toInt());
            _client->setVirtualHost(info->value("amqp_vhost", "/"));
            _client->setUser(info->value("amqp_user", "guest"));
            _client->setPassword(info->value("amqp_password", "guest"));
        }
        _client->open();

        _queue = _client->createQueue();
        _queue->declare("new-queue", Queue::Durable);

        connect(_queue, SIGNAL(declared()), this, SLOT(declared()));
        connect(_queue, SIGNAL(messageReceived(QAMQP::Queue * )), this, SLOT(newMessage(QAMQP::Queue *)));

    }

    void AMQPWatcher::declared()
    {
        _queue->setQOS(0,1);
        _queue->bind("new-exchange", "");
        _queue->consume();
    }

    void AMQPWatcher::newMessage(QAMQP::Queue * q)
    {
        // Retrieve message
        QAMQP::MessagePtr message = q->getMessage();
        qDebug() << "New message received: " << message->payload;

        // Ack to server
        q->ack(message);

        QJsonObject response = QJsonDocument::fromJson(message->payload).object();

        QString type = response.value(QString("type")).toString();
        QJsonObject jsonMessage = response.value("message").toObject();

        if (type == QString("new_message"))
        {
            QString from = jsonMessage.value(QString("from")).toString();
            QString to = jsonMessage.value(QString("to")).toString();
            QString body = jsonMessage.value(QString("body")).toString();

            emit messageReceived(new MessageInfo(from, to, body));
        }
    }
}
}
