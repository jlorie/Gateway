#ifndef DEVICEINFO_HPP
#define DEVICEINFO_HPP

#include <QString>

struct GSMDeviceInfo
{
    QString deviceId;
    QString devicePort;
    QString deviceName;
    QString manufacturer;
    QString model;
    QString deviceDriver;
    bool isEnabled;
};

#endif // DEVICEINFO_HPP
