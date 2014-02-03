#ifndef DRIVERIMP_HPP
#define DRIVERIMP_HPP

#include <QtPlugin>
#include <QObject>

#include <include/DriverInterface.hpp>
#include "Device.hpp"

using Gateway::Driver::Device;

class DriverImp : public QObject, public DriverInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Gateway.DriverInterface")
    Q_INTERFACES(DriverInterface)

    IDevice *newDevice(const DeviceInfo &info)
    {
        Device *device = new Device(info);
        if (!device->initialize())
        {
            delete device;
            device = 0;
        }

        return device;
    }

    QString driverName() const
    {
        return QString("GammuGSMDriver");
    }

    QString description() const
    {
        return QString("Driver for multiples phones or gsm modems using Gammu library");
    }
};

#endif // DRIVERIMP_HPP
