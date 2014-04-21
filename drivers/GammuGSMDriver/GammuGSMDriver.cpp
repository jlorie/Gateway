#include "GammuGSMDriver.hpp"

#include "Device.hpp"

namespace Gateway
{
namespace Driver
{
    IDevice *OldGammuGSMDriver::newDevice(const DeviceInfo &info)
    {
        Device *device = new Device(info);
        if (!device->initialize())
        {
            delete device;
            device = 0;
        }

        return device;
    }

    QString OldGammuGSMDriver::driverName() const
    {
        return QString("GammuGSMDriver");
    }

    QString OldGammuGSMDriver::description() const
    {
        return QString("Driver for multiples phones or gsm modems using Gammu library");
    }

    Q_EXPORT_PLUGIN2(oldGammuGSMDriver, OldGammuGSMDriver)
}
}
