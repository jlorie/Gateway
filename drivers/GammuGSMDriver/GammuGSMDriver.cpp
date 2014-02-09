#include "GammuGSMDriver.hpp"

#include "Device.hpp"

namespace Gateway
{
namespace Driver
{
    IDevice *GammuGSMDriver::newDevice(const DeviceInfo &info)
    {
        Device *device = new Device(info);
        if (!device->initialize())
        {
            delete device;
            device = 0;
        }

        return device;
    }

    QString GammuGSMDriver::driverName() const
    {
        return QString("GammuGSMDriver");
    }

    QString GammuGSMDriver::description() const
    {
        return QString("Driver for multiples phones or gsm modems using Gammu library");
    }

    Q_EXPORT_PLUGIN2(gammuGSMDriver, GammuGSMDriver)
}
}
