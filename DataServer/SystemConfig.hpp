#ifndef SYSTEMCONFIG_HPP
#define SYSTEMCONFIG_HPP

#include <include/DeviceInfo.hpp>

#include <QObject>
#include <QSettings>

typedef QList<DeviceInfo> DeviceInfoList;

class SystemConfig : public QObject
{
    Q_OBJECT
public:
    explicit SystemConfig(QObject *parent = 0);

    DeviceInfoList devicesInfo();

private:
    void loadSettings();

private:
    DeviceInfoList _devicesInfo;
    QSettings *_settings;

};

#endif // SYSTEMCONFIG_HPP
