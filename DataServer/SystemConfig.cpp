#include "SystemConfig.hpp"

SystemConfig::SystemConfig(QObject *parent) :
    QObject(parent)
{
    _settings = new QSettings("Cubania Team", "Gateway");
}

DeviceInfoList SystemConfig::devicesInfo()
{
    loadSettings();
    return _devicesInfo;
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
