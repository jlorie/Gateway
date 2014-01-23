#ifndef DEVICEBROWSER_HPP
#define DEVICEBROWSER_HPP

#include <DeviceInfo.hpp>

#include <QSerialPortInfo>
#include <QSettings>
#include <QList>
#include <QMap>
#include <QObject>

typedef QList<GSMDeviceInfo *> DeviceInfoList;
typedef QMap<QString, GSMDeviceInfo *> DeviceInfoMap;

class DeviceBrowser
{
public:
    DeviceBrowser();
    ~DeviceBrowser();

    DeviceInfoList infoList();

    void loadSettings();
    void saveSettings();

    void sync();
    void print();

private:
    DeviceInfoMap _devicesInfo;
    QSettings *_settings;

    DeviceInfoList _devicesToAdd;
    DeviceInfoList _devicesToRemove;
};

#endif // DEVICEBROWSER_HPP
