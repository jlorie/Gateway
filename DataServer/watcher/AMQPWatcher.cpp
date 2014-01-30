#include "AMQPWatcher.hpp"
#include <common/SMS.hpp>

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

namespace Gateway
{

    const QString amqpHost = QString("hyena.rmq.cloudamqp.com");
    const uint amqpPort = 5672;
    const QString amqpVhost = QString("dfvshlxn");
    const QString amqpUser = QString("dfvshlxn");
    const QString amqpPassword = QString("OZE08m61Q_QcN01owJSr4z0eo5cM-OUr");

    using QAMQP::Queue;

    AMQPWatcher::AMQPWatcher(QObject *parent) :
        RemoteWatcher(parent)
    {
        _client = new QAMQP::Client(this);
        {
            _client->setHost(amqpHost);
            _client->setPort(amqpPort);
            _client->setVirtualHost(amqpVhost);
            _client->setUser(amqpUser);
            _client->setPassword(amqpPassword);
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

            emit messageReceived(new SMS(from, to, body));
        }
        else
        if (type == QString("status_message"))
        {
            uint messageId = jsonMessage.value(QString("id")).toString().toInt();
            QString status = jsonMessage.value(QString("status")).toString();

            if (status == QString("sent"))
                emit messageSent(messageId);
            else
                emit messageRefused(messageId);
        }
    }
}
