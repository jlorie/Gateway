#include "XmppWatcher.hpp"

#include <include/DataStructures/MessageInfo.hpp>

#include <QVariantMap>
#include <QString>

#include <qjson/parser.h>
#include "QXmppLogger.h"

namespace Gateway
{
namespace Watcher
{
    XmppWatcher::XmppWatcher(WatcherInfo *info)
        :_info(info)
    {
        bool check = connect(&_xmppClient, SIGNAL(messageReceived(QXmppMessage)),
                             this, SLOT(onMessageReceived(QXmppMessage)));

        connect(&_xmppClient, SIGNAL(connected()), this, SLOT(onConnect()));
        Q_ASSERT(check);
        Q_UNUSED(check);

        _xmppClient.logger()->setLoggingType(QXmppLogger::StdoutLogging);

    }

    void XmppWatcher::start()
    {
        QXmppConfiguration config;
        config.setJid(_info->value("xmpp_jid"));
        config.setHost(_info->value("xmpp_host"));
        config.setPort(_info->value("xmpp_port").toInt());
        config.setPassword(_info->value("xmpp_password"));

        config.setStreamSecurityMode(QXmppConfiguration::SSLOnly);
        config.setKeepAliveInterval(60);
        config.setAutoAcceptSubscriptions(true);

        _xmppClient.connectToServer(config);
    }

    void XmppWatcher::stop()
    {
        _xmppClient.disconnectFromServer();
    }

    void XmppWatcher::onMessageReceived(const QXmppMessage& message)
    {
        QString body = message.body();

        if (!body.isEmpty())
        {
            QJson::Parser parser;
            QVariantMap response = parser.parse(body.toLatin1()).toMap();

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

    void XmppWatcher::onConnect()
    {
        qDebug("....... xmpp connected");
//        sendPacket(QXmppMessage("", QString("sms_bot1@jabber.me"), "---> Hello world!!!"));
    }
}
}
