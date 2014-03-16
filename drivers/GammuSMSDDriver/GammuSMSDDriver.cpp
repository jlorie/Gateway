#include "GammuSMSDDriver.hpp"
#include <QProcess>

namespace Gateway
{
    IDevice *GammuSMSDDriver::newDevice(const DeviceInfo &info)
    {
        Device *device = new Device(info);
        if (!device->initialize())
        {
            delete device;
            device = 0;
        }

        return device;
    }

    QString GammuSMSDDriver::driverName() const
    {
        return QString("GammuSMSDDriver");
    }

    QString GammuSMSDDriver::description() const
    {
        return QString("Driver for GSM device through gammu-smsd command lines");
    }

    Q_EXPORT_PLUGIN2(smsdDriver, GammuSMSDDriver)
}
