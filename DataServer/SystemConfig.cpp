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
        DeviceInfoList devicesInfo;
        QString logsPath(_settings->value("logs_path").toString());        
        QString logsEnabled(_settings->value("devices_logs_enabled").toString());

        QString smsPath(_settings->value("sms_path").toString());

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

                //insert devices logs
                devInfo.insert("logs_enabled", logsEnabled);

                QString number(devInfo.value("device_phonenumber"));
                devInfo.insert("log_file", logsPath + "/" + "number" + number + ".log");

                //insert sms path -- for GammuSMSDDriver
                devInfo.insert("sms_path", smsPath + "/"+ "number" + number);

                devicesInfo.append(devInfo);
                _settings->endGroup();
            }
        }
        _settings->endGroup();


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
