#include "SystemEngine.hpp"

#include <include/WatcherInterface.hpp>

#include <SystemConfig.hpp>
#include <DeviceManager.hpp>
#include <RemoteStorage.hpp>
#include <DriverManager.hpp>

#include <QPluginLoader>



#include <include/DataStructures/MessageInfo.hpp>

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

        registerWatcher();
        if (_watcher)
            connect(_watcher, SIGNAL(messageReceived(const IMessage*)), this, SLOT(redirectMessage(const IMessage*)));
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

    void SystemEngine::registerWatcher()
    {
WatcherInfo *watcherInfo = SystemConfig::instance()->watcherInfo();
{
    //AMQP
//    watcherInfo.insert("watcher_filename", "/home/lorie/workspace/Projects/_qt-builds/libs/libAMQPWatcher.so");
//    watcherInfo.insert("amqp_host", "hyena.rmq.cloudamqp.com");
//    watcherInfo.insert("amqp_port", "5672");
//    watcherInfo.insert("amqp_vhost", "dfvshlxn");
//    watcherInfo.insert("amqp_user", "dfvshlxn");
//    watcherInfo.insert("amqp_password", "OZE08m61Q_QcN01owJSr4z0eo5cM-OUr");

    //Http
//    watcherInfo->insert("watcher_filename", "/home/lorie/workspace/Projects/_qt-builds/libs/libHttpWatcher.so");
//    watcherInfo->insert("http_url", "http://cubania.info/app.php/api/sms/");
//    watcherInfo->insert("http_username", "dfvshlxn");
//    watcherInfo->insert("http_password", "OZE08m61Q_QcN01owJSr4z0eo5cM-OUr");
//    watcherInfo->insert("http_poll_interval", "5");

//    watcherInfo->insert("sms_last_id", "165");
}

        QPluginLoader loader(watcherInfo->value("watcher_filename"));
        QObject *library = loader.instance();
        if (library)
        {
            WatcherInterface *watcherProvider = qobject_cast<WatcherInterface *>(library);
            if (watcherProvider)
            {
                qDebug("Registering watcher: %s", qPrintable(watcherProvider->watcherName()));
                qDebug(">>    %s", qPrintable(watcherProvider->description()));

                _watcher = watcherProvider->newWatcher(watcherInfo);
                if (!_watcher)
                {
                    qWarning("Error initializing watcher %s",
                             qPrintable(watcherInfo->value("watcher_filename")));
                }
            }
        }
        else
        {
            QString error = loader.errorString();
            error = error.mid(error.lastIndexOf(".so:") + 4);
            qWarning("Cannot load watcherLibrary %s: %s",
                     qPrintable(watcherInfo->value("watcher_filename")), qPrintable(error.toLatin1()));
        }
    }
}
