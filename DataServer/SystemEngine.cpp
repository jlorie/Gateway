#include "SystemEngine.hpp"

#include <include/WatcherInterface.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include <SystemConfig.hpp>
#include <DeviceManager.hpp>
#include <RemoteStorage.hpp>
#include <DriverManager.hpp>
#include <WatcherManager.hpp>

#include <QNetworkProxy>
#include <QPluginLoader>

namespace Gateway
{
    SystemEngine::SystemEngine()
    {
        setProxy();

        RemoteStorage::initialize();
        DriverManager::initialize();
        DeviceManager::initialize();
        WatcherManager::initialize();


        RemoteStorage *storage = RemoteStorage::instance();
        DeviceManager *devManager = DeviceManager::instance();

        connect(devManager, SIGNAL(newMessageReceived(const IMessage*)),
                storage, SLOT(dispatchMessage(const IMessage*)));

        _lastId = -1;
        qDebug("Fetching pending messages from main server ...");
        MessageList pendingMessages(storage->pendingMessages());
        if (!pendingMessages.empty())
        {
            foreach (IMessage *message, pendingMessages)
            {
                redirectMessage(message);
            }
        }

        _watcher = WatcherManager::instance()->activeWatcher();

        if (_watcher)
        {
            connect(_watcher, SIGNAL(messageReceived(const IMessage*)),
                    this, SLOT(redirectMessage(const IMessage*)));

            _watcher->start();
        }
    }

    void SystemEngine::redirectMessage(const IMessage *message)
    {
        qDebug(">> Redirecting message");
        qDebug("----> from: %s", qPrintable(message->from()));
        qDebug("----> to: %s", qPrintable(message->to()));
        qDebug("----> body: %s", qPrintable(message->body()));

        if (message->id() <= _lastId && _lastId > 0)
        {
            qWarning("Ignoring message with id %lld has already processed", message->id());
            return;
        }
        _lastId = message->id();

        DeviceManager *devManager = DeviceManager::instance();
        if (!devManager)
        {
            qWarning("Could not find instance for DeviceManager");
            return;
        }

        NumberInfo *phoneNumber = devManager->phoneForNumber(message->from());
        if (phoneNumber)
        {
            IDevice *sender = phoneNumber->device();

            if (!sender)
            {
                qWarning("Could not find instance for device with number %s", qPrintable(message->from()));
                RemoteStorage::instance()->notifyMessageStatus(message->id(), stFailed);

                return;
            }

            sender->sendMessage(message);
        }
        else
        {
            qWarning("Number %s has not been registered", qPrintable(message->from()));
            RemoteStorage::instance()->notifyMessageStatus(message->id(), stFailed);
        }
    }

    void SystemEngine::setProxy()
    {
        SystemConfig *config = SystemConfig::instance();

        if (config->value("proxy_enabled").toString() == "1")
        {
            QString hostName(config->value("proxy_hostname").toString());
            uint port(config->value("proxy_port").toInt());

            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(hostName);
            proxy.setPort(port);

            QNetworkProxy::setApplicationProxy(proxy);
        }
    }
}
