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

    DeviceInfoList SystemConfig::devicesInfo()
    {
        return _devicesInfo;
    }

    QVariant SystemConfig::value(const QString &key, const QVariant &defaultValue) const
    {
        return _settings->value(key, defaultValue);
    }

    SystemConfig::SystemConfig()
    {
        _settings = new QSettings("Cubania Team", "Gateway");
        loadSettings();
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
    }

}
