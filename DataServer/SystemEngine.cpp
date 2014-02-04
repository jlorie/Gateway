#include "SystemEngine.hpp"

#include <include/WatcherInterface.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include <SystemConfig.hpp>
#include <DeviceManager.hpp>
#include <RemoteStorage.hpp>
#include <DriverManager.hpp>
#include <WatcherManager.hpp>

#include <QPluginLoader>

namespace Gateway
{
    SystemEngine::SystemEngine()
    {
        SystemConfig::initialize();

        RemoteStorage::initialize();
        DriverManager::initialize();
        DeviceManager::initialize();
        WatcherManager::initialize();


        RemoteStorage *storage = RemoteStorage::instance();
        DeviceManager *devManager = DeviceManager::instance();

        connect(devManager, SIGNAL(newMessageReceived(const IMessage*)),
                storage, SLOT(dispatchMessage(const IMessage*)));

        _lastId = -1;
        MessageList pendingMessages(storage->pendingMessages());
        if (!pendingMessages.empty())
        {
            qDebug("Processing pending messages from main server ...");

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
        if (message->id() < _lastId && _lastId > 0)
        {
            qWarning("Ignoring message with id %lld has already processed", message->id());
            return;
        }

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
                RemoteStorage *storage = RemoteStorage::instance();
                storage->notifyMessageStatus(message->id(), stFailed);

                return;
            }

            sender->sendMessage(message);
            _lastId = message->id();
        }
        else
        {
            qWarning("Number %s has not been registered", qPrintable(message->from()));
        }
    }
}
