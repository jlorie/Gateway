#include "SystemEngine.hpp"

#include <include/WatcherInterface.hpp>
#include <include/DataStructures/MessageInfo.hpp>

#include <SystemConfig.hpp>
#include <DeviceManager.hpp>
#include <RemoteStorage.hpp>
#include <DriverManager.hpp>

#include <QPluginLoader>

namespace Gateway
{
    SystemEngine::SystemEngine()
    {
        SystemConfig::initialize();

        RemoteStorage::initialize();
        DriverManager::initialize();
        DeviceManager::initialize();

        RemoteStorage *storage = RemoteStorage::instance();
        DeviceManager *devManager = DeviceManager::instance();

        connect(devManager, SIGNAL(newMessageReceived(const IMessage*)),
                storage, SLOT(dispatchMessage(const IMessage*)));

        //initialize
        {
            registerWatcher();
            _watcher = _defaultWatcher;
            _lastId = 0;

            MessageList pendingMessages(_defaultWatcher->pendingMessages());
            foreach (IMessage *message, pendingMessages)
            {
                redirectMessage(message);
                _lastId = message->id();
            }
        }

        if (_watcher)
            connect(_watcher, SIGNAL(messageReceived(const IMessage*)), this, SLOT(redirectMessage(const IMessage*)));
    }

    void SystemEngine::redirectMessage(const IMessage *message)
    {
        if (message->id() < _lastId)
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

        IPhoneNumber *sender = devManager->phoneForNumber(message->from());
        if (!sender)
        {
            qWarning("Could not find instance for number %s", qPrintable(message->from()));
            return;
        }

        sender->sendMessage(message);
    }

    void SystemEngine::registerWatcher()
    {
        SystemConfig *config = SystemConfig::instance();
        qDebug("Registering watchers from %s", qPrintable(config->mainInfo()->value("lib_path")));

        //registering default watcher
        {
            _defaultWatcher = new HttpWatcher;
            qDebug("----> HttpWatcher registered");
        }

        WatcherInfoList infoList = config->watchersInfo();

        foreach (WatcherInfo *watcherInfo, infoList)
        {
            QString fileName; // generating fileName
            {
                fileName.append(config->mainInfo()->value("lib_path"));
                if (!fileName.endsWith('/'))
                    fileName.append("/");

                fileName.append(QString("lib%1.so").arg(watcherInfo->value("watcher_name")));
            }

            QPluginLoader loader(fileName);
            QObject *library = loader.instance();
            if (library)
            {
                WatcherInterface *watcherProvider = qobject_cast<WatcherInterface *>(library);
                if (watcherProvider)
                {
                    qDebug("----> %s registered, %s", qPrintable(watcherProvider->watcherName()),
                           qPrintable(watcherProvider->description()));
                }
            }
            else
            {
                QString error = loader.errorString();
                error = error.mid(error.lastIndexOf(".so:") + 4);
                qWarning("Cannot load watcherLibrary %s: %s",
                         qPrintable(fileName), qPrintable(error.toLatin1()));
            }
        }
    }
}
