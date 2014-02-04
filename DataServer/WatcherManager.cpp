#include "WatcherManager.hpp"
#include "SystemConfig.hpp"

#include <QPluginLoader>

namespace Gateway
{
    WatcherManager *WatcherManager::_instance = 0;

    bool WatcherManager::initialize()
    {
        if (!_instance)
        {
            _instance = new WatcherManager;
        }

        return true;
    }

    WatcherManager *WatcherManager::instance()
    {
        return _instance;
    }

    void WatcherManager::destroyInstance()
    {
        delete _instance;
    }

    IWatcher *WatcherManager::activeWatcher()
    {
        return _activeWatcher;
    }

    void WatcherManager::setWatcher(const QString &watcherName)
    {
        qDebug("Setting up watcher %s", qPrintable(watcherName));

        SystemConfig *config = SystemConfig::instance();
        WatcherInfoList infoList = config->watchersInfo();

        //Find info for watcher_name
        WatcherInfo *watcherInfo = 0;
        foreach (WatcherInfo *info, infoList)
        {
            if (info->value("watcher_name") == watcherName)
            {
                watcherInfo = info;
                break;
            }
        }

        if (!watcherInfo)
        {
            qWarning("Not info found for %s", qPrintable(watcherName));
            return;
        }

        _activeWatcher = 0;
        foreach (WatcherInterface *library, _libraries)
        {
            if (library->watcherName() == watcherName)
            {
                _activeWatcher = library->newWatcher(watcherInfo);
                break;
            }
        }
    }

    WatcherManager::WatcherManager()
    {
        QString watcherName(SystemConfig::instance()
                            ->mainInfo()
                            ->value("watcher", "HttpWatcher"));

        registerWatcher();
        setWatcher(watcherName);
    }

    void WatcherManager::registerWatcher()
    {
        SystemConfig *config = SystemConfig::instance();
        qDebug("Registering watchers from %s", qPrintable(config->mainInfo()->value("lib_path")));

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

                    _libraries.append(watcherProvider);
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
