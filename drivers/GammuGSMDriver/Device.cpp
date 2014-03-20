#include <include/DataStructures/MessageInfo.hpp>

#include "Device.hpp"
#include <QMetaType>

namespace Gateway
{
namespace Driver
{

    Device::Device(const DeviceInfo &info)
        :_info(info)
    {
        _smsd = new GammuSMSD(_info.value(QString("serial_port")));
        _number = _info.value("device_phonenumber");

        connect(_smsd, SIGNAL(messageReceived(QString,QString)),
                    this, SLOT(onMessageReceived(QString,QString)));

        qRegisterMetaType<MessageStatus>("MessageStatus");
        connect(_smsd, SIGNAL(messageStatusChanged(qlonglong,MessageStatus)),
                this, SIGNAL(messageStatusChanged(qlonglong,MessageStatus)));

        connect(_smsd, SIGNAL(connectionTerminated()), this, SIGNAL(connectionClosed()));
    }

    Device::~Device()
    {
        _smsd->stop();
        delete _smsd;
    }

    bool Device::initialize()
    {
        bool result(true);
        if (_smsd->init())
        {
            _smsd->start();
        }
        else
        {
            result = false;
        }

        return result;
    }

    QString Device::deviceId()
    {
        return _smsd->imsi();
    }

    void Device::sendMessage(const IMessage *message)
    {
        _smsd->injectSms(message->to(), message->body(), message->id());
    }

    MessageList Device::pendingMessages() const
    {
        MessageList messages;
        return messages;
    }

    void Device::onMessageReceived(const QString &from, const QString &body)
    {
        emit newMessageReceived(new MessageInfo(from, _number, body));
    }
}
}
