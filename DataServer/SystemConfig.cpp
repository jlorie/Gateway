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
        return _devicesInfo.values();
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

    void SystemConfig::checkNewDevice(const QString &path)
    {
        QDir configDir(path);

        // new devices
        QStringList newDevices;
        foreach (QString fileName, configDir.entryList(QDir::Files|QDir::NoDotDot))
        {
            QString absoluteFileName(configDir.absoluteFilePath(fileName));
            if (!_devicesInfo.contains(absoluteFileName))
            {
                newDevices.append(absoluteFileName);
            }
        }

        // devices to remove
        QStringList oldDevices;
        foreach (QString fileName, _devicesInfo.keys())
        {
            if (!configDir.exists(fileName))
            {
                oldDevices.append(fileName);
            }
        }

        foreach (QString fileName, newDevices)
        {
            DeviceInfo devInfo(loadDeviceInfo(fileName));
            if (!devInfo.isEmpty())
            {
                emit newDeviceFound(devInfo);

                _devicesInfo.insert(fileName, devInfo);
            }
        }

        foreach (QString fileName, oldDevices)
        {
            DeviceInfo devInfo(_devicesInfo.take(fileName));
            emit deviceRemoved(devInfo.value("device_imei"));
        }
    }

    SystemConfig::SystemConfig(const QString &configFile)
    {
        if (configFile.isEmpty())
            _settings = new QSettings("Cubania Team", "Gateway");
        else
            _settings = new QSettings(configFile, QSettings::IniFormat);

        // browsing device configuration files
        {
            QString devConfPath(_settings->value("device_conf_path", "/tmp/devices").toString());

            QDir devDir(devConfPath);
            foreach (QString fileName, devDir.entryList(QDir::Files|QDir::NoDotDot))
            {
                QString configFileName(devDir.absoluteFilePath(fileName));
                DeviceInfo devInfo(loadDeviceInfo(configFileName));

                if (!devInfo.isEmpty())
                {
                   _devicesInfo.insert(configFileName, devInfo);
                }
            }

            _deviceWatcher.addPath(devConfPath);
            connect(&_deviceWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(checkNewDevice(QString)));
        }
    }

    SystemConfig::~SystemConfig()
    {
    }

    DeviceInfo SystemConfig::loadDeviceInfo(const QString &configFileName) const
    {
        DeviceInfo devInfo;
        QSettings deviceSettings(configFileName, QSettings::IniFormat);

        foreach (QString key, deviceSettings.allKeys())
        {
            devInfo.insert(key, deviceSettings.value(key).toString());
        }

        return devInfo;
    }
}
