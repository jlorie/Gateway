#include "XmppWatcher.hpp"

#include <SystemConfig.hpp>
#include <qjson/parser.h>

#include <qxmpp/QXmppMessage.h>
#include <qxmpp/QXmppLogger.h>

namespace Gateway
{
namespace Watcher
{
    XmppWatcher::XmppWatcher(WatcherInfo *info)
        :_info(info)
    {
        bool check = connect(&_xmppClient, SIGNAL(messageReceived(QXmppMessage)),
                             this, SLOT(messageReceived(QXmppMessage)));

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
        config.setKeepAliveInterval(0);

        _xmppClient.connectToServer(config);
    }

    void XmppWatcher::stop()
    {
        _xmppClient.disconnectFromServer();
    }

    void XmppWatcher::messageReceived(const QXmppMessage& message)
    {
        QString from = message.from();
        QString msg = message.body();

        qDebug("Message received: %s from %s", qPrintable(msg), qPrintable(from));

//        sendPacket(QXmppMessage("", from, "Your message: " + msg));
    }

    void XmppWatcher::onConnect()
    {
        qDebug("....... xmpp connected");
//        sendPacket(QXmppMessage("", QString("sms_bot1@jabber.me"), "---> Hello world!!!"));
    }
}
}
