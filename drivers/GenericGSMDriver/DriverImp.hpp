#ifndef DRIVERIMP_HPP
#define DRIVERIMP_HPP

#include <QtPlugin>
#include <QObject>

#include <include/DriverInterface.hpp>
#include "Device.hpp"

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
        return QString("GenericGSMDriver");
    }

    QString description() const
    {
        return QString("Generic controller for gsm devices, mobile phones o gsm modems.");
    }
};

#endif // DRIVERIMP_HPP
