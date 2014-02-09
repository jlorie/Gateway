#include "AMQPWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QString>
#include <qjson/parser.h>
#include <QVariantMap>

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
    }

    void AMQPWatcher::start()
    {
        _queue = _client->createQueue();
        _queue->declare("new-queue", Queue::Durable);

        connect(_queue, SIGNAL(declared()), this, SLOT(declared()));
        connect(_queue, SIGNAL(messageReceived(QAMQP::Queue * )), this, SLOT(newMessage(QAMQP::Queue *)));
    }

    void AMQPWatcher::stop()
    {
        //TODO dejar de recibir mensajes
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

        QJson::Parser parser;
        QVariantMap response = parser.parse(message->payload).toMap();

        QString type(response.value("type").toString());
        QVariantMap sms = response.value("message").toMap();

        if (type == QString("new_message"))
        {
            qlonglong id = sms.value("id").toLongLong();
            QString from = sms.value("from").toString();
            QString to = sms.value("to").toString();
            QString body = sms.value("body").toString();

            emit messageReceived(new MessageInfo(from, to, body, id));
        }
    }
}
}
