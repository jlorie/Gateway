#ifndef DEVICEINFO_HPP
#define DEVICEINFO_HPP

#include <QString>

namespace Gateway
{
    struct DeviceInfo
    {
        qlonglong deviceID;
        QString name;
        QString protocol;
        QString manufacturer;

        bool enabled;
    };
}

#endif // DEVICEINFO_HPP
