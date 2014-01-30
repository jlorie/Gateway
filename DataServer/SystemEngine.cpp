#include "SystemEngine.hpp"

#include <SystemConfig.hpp>
#include <DeviceManager.hpp>
#include <RemoteStorage.hpp>
#include <DriverManager.hpp>


#include <common/SMS.hpp>

namespace Gateway
{
    SystemEngine::SystemEngine()
    {
        SystemConfig::initialize();
        RemoteStorage::initialize();
        DriverManager::initialize();
        DeviceManager::initialize();

//        _watcher = new AMQPWatcher();

        RemoteStorage *storage = RemoteStorage::instance();
        DeviceManager *devManager = DeviceManager::instance();

        connect(devManager, SIGNAL(newMessageReceived(const IMessage*)),
                storage, SLOT(dispatchMessage(const IMessage*)));

//        connect(_watcher, SIGNAL(messageReceived(const IMessage*)), this, SLOT(redirectMessage(const IMessage*)));
    }

    void SystemEngine::redirectMessage(const IMessage *message)
    {
        DeviceManager *devManager = DeviceManager::instance();
        if (!devManager)
        {
            qDebug("Could not find instance for DeviceManager");
            return;
        }

        IPhoneNumber *sender = devManager->phoneForNumber(message->from());
        if (!sender)
        {
            qDebug("Could not find instance for number %s", qPrintable(message->from()));
            return;
        }

        sender->sendMessage(message);
    }
}
