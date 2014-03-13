#include "GammuSMSDDriver.hpp"
#include <QProcess>

namespace Gateway
{
    IDevice *GammuSMSDDriver::newDevice(const DeviceInfo &info)
    {
        //killing previous instances of gammu-smsd
        QProcess::execute("killall gammu-smsd");

        return new Device(info);
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
