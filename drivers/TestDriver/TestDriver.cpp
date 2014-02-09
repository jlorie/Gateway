#include "TestDriver.hpp"

namespace Gateway
{
    IDevice *TestDriver::newDevice(const DeviceInfo &info)
    {
        return new Device(info);
    }

    QString TestDriver::driverName() const
    {
        return QString("TestDriver");
    }

    QString TestDriver::description() const
    {
        return QString("TestDriver");
    }

    Q_EXPORT_PLUGIN2(testDriver, TestDriver)
}
