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

        if (storage)
        {
            _lastId = -1;
            if (devManager)
            {
                connect(devManager, SIGNAL(newMessageReceived(const IMessage*)),
                        storage, SLOT(dispatchMessage(const IMessage*)));

                devManager->loadDevices();
            }

            qDebug(">> Fetching pending messages from main server ...");
            MessageList pendingMessages(storage->pendingMessages());

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

//        storage->dispatchMessage(new MessageInfo("+584120884437", "+584140937970",
//                                                 "Very long example Gammu message to show how to construct contatenated."));
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
        ProxyInfo info = SystemConfig::instance()->proxyInfo();

        if (info.value("proxy_enabled").toInt() == 1)
        {
            QString hostName(info.value("proxy_hostname"));
            uint port(info.value("proxy_port").toInt());

            QString username = (info.value("proxy_username"));
            QString password = (info.value("proxy_password"));

            QNetworkProxy proxy;
            proxy.setType(QNetworkProxy::HttpProxy);
            proxy.setHostName(hostName);
            proxy.setPort(port);

            if (!username.isEmpty())
            {
                proxy.setUser(username);
                proxy.setPassword(password);
            }

            QNetworkProxy::setApplicationProxy(proxy);
        }
    }
}
