#ifndef IDEVICEFACTORY_HPP
#define IDEVICEFACTORY_HPP

#include <QtPlugin>
#include <QString>

#include "IDevice.hpp"
#include "DataStructures/DeviceInfo.hpp"

class DriverInterface
{
    public:
        virtual IDevice *newDevice(const DeviceInfo &info) = 0;

        virtual QString driverName() const  = 0;
        virtual QString description() const  = 0;
};


Q_DECLARE_INTERFACE(DriverInterface, "Gateway.DriverInterface")


#endif // IDEVICEFACTORY_HPP
