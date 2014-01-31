#include "SystemConfig.hpp"

namespace Gateway
{
    SystemConfig *SystemConfig::_instance = 0;

    bool SystemConfig::initialize()
    {
        bool result(true);

        if (!_instance)
        {
            qDebug(">> Initializing SystemCofig");
            _instance = new SystemConfig;
            qDebug(">> SystemCofig initialized");
        }

        return result;
    }

    SystemConfig *SystemConfig::instance()
    {
        return _instance;
    }

    void SystemConfig::destroyInstance()
    {
        delete _instance;
    }

    DeviceInfoList SystemConfig::devicesInfo() const
    {
        return _devicesInfo;
    }

    WatcherInfo *SystemConfig::watcherInfo() const
    {
        return _watcherInfo;
    }

    QVariant SystemConfig::value(const QString &key, const QVariant &defaultValue) const
    {
        return _settings->value(key, defaultValue);
    }

    SystemConfig::SystemConfig()
        :_watcherInfo(0)
    {
        _settings = new QSettings("Cubania Team", "Gateway");
        loadSettings();
    }

    SystemConfig::~SystemConfig()
    {
        saveSettings();
    }

    void SystemConfig::loadSettings()
    {
        _settings->beginGroup(QString("Devices"));
        {
            foreach (QString group, _settings->childGroups())
            {
                DeviceInfo devInfo;
                _settings->beginGroup(group);

                foreach (QString key, _settings->allKeys())
                {
                    devInfo.insert(key, _settings->value(key).toString());
                }

                _devicesInfo.append(devInfo);

                _settings->endGroup();
            }
        }
        _settings->endGroup();

        _settings->beginGroup(QString("Watchers"));
        {
            if (_watcherInfo)
                delete _watcherInfo;

            _watcherInfo = new WatcherInfo;
            foreach (QString key, _settings->allKeys())
            {
                _watcherInfo->insert(key, _settings->value(key).toString());
            }
        }
        _settings->endGroup();
    }

    void SystemConfig::saveSettings()
    {
        _settings->beginGroup("Watchers");
        {
            foreach (QString key, _watcherInfo->keys())
            {
                _settings->setValue(key, _watcherInfo->value(key));
            }
        }
        _settings->endGroup();
    }

}
