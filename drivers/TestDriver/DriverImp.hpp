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
        return new Device(info);
    }

    QString driverName() const
    {
        return QString("TestDriver");
    }

    QString description() const
    {
        return QString("TestDriver");
    }
};

#endif // DRIVERIMP_HPP
