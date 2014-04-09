#include "SystemConfig.hpp"

#include <QDir>

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
        DeviceInfoList devicesInfo;
        QString devConfPath(_settings->value("device_conf_path").toString());

        QDir devDir(devConfPath);
        foreach (QString fileName, devDir.entryList(QDir::Files|QDir::NoDotDot))
        {
            DeviceInfo devInfo;
            QSettings deviceSettings(devDir.absoluteFilePath(fileName), QSettings::IniFormat);

            foreach (QString key, deviceSettings.allKeys())
            {
                devInfo.insert(key, deviceSettings.value(key).toString());
            }

            if (!devInfo.isEmpty())
            {
                devicesInfo.append(devInfo);
            }
        }

        return devicesInfo;
    }

    WatcherInfoList SystemConfig::watchersInfo() const
    {
        WatcherInfoList _watchersInfo;

        _settings->beginGroup(QString("Watchers"));
        {
            foreach (QString group, _settings->childGroups())
            {
                _settings->beginGroup(group);
                {
                    WatcherInfo watcher;
                    foreach (QString key, _settings->allKeys())
                    {
                        watcher.insert(key, _settings->value(key).toString());
                    }

                    _watchersInfo.append(watcher);
                }
                _settings->endGroup();
            }
        }
        _settings->endGroup();

        return _watchersInfo;
    }

    ProxyInfo SystemConfig::proxyInfo() const
    {
        ProxyInfo proxy;
        _settings->beginGroup(QString("Proxy"));
        {
            foreach (QString key, _settings->allKeys())
            {
                proxy.insert(key, _settings->value(key).toString());
            }
        }
        _settings->endGroup();

        return proxy;
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
    }

    SystemConfig::~SystemConfig()
    {
    }
}
