#include "SystemConfig.hpp"

namespace Gateway
{
    SystemConfig *SystemConfig::_instance = 0;

    bool SystemConfig::initialize(const QString &configFile)
    {
        bool result(true);

        if (!_instance)
        {
            qDebug("Initializing SystemCofig");
            _instance = new SystemConfig(configFile);
            qDebug("----> SystemCofig initialized");
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

    WatcherInfoList SystemConfig::watchersInfo() const
    {
        return _watchersInfo;
    }

    MainInfo *SystemConfig::mainInfo() const
    {
        return _mainInfo;
    }

    QVariant SystemConfig::value(const QString &key, const QVariant &defaultValue) const
    {
        return _settings->value(key, defaultValue);
    }

    SystemConfig::SystemConfig(const QString &configFile)
    {
        if (configFile.isEmpty())
            _settings = new QSettings("Cubania Team", "Gateway");
        else
            _settings = new QSettings(configFile, QSettings::IniFormat);

        loadSettings();
    }

    SystemConfig::~SystemConfig()
    {
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
            foreach (QString group, _settings->childGroups())
            {
                _settings->beginGroup(group);
                {
                    WatcherInfo * watcher = new WatcherInfo;
                    foreach (QString key, _settings->allKeys())
                    {
                        watcher->insert(key, _settings->value(key).toString());
                    }

                    _watchersInfo.append(watcher);
                }
                _settings->endGroup();
            }
        }
        _settings->endGroup();

        _settings->beginGroup(QString("Main"));
        {
            _mainInfo = new MainInfo;
            foreach (QString key, _settings->allKeys())
            {
                _mainInfo->insert(key, _settings->value(key).toString());
            }
        }
        _settings->endGroup();
    }
}
