#include "GammuSMSDDriver.hpp"
#include <QProcess>

namespace Gateway
{
    GammuSMSDDriver::GammuSMSDDriver()
    {
        QProcess killGammu;
        killGammu.start("killall gammu-smsd");
        killGammu.waitForFinished();
    }

    IDevice *GammuSMSDDriver::newDevice(const DeviceInfo &info)
    {
        return new Device(info);
    }

    QString GammuSMSDDriver::driverName() const
    {
        return QString("OldSMSDDriver");
    }

    QString GammuSMSDDriver::description() const
    {
        return QString("Old Driver for GSM device through gammu-smsd command lines");
    }

    Q_EXPORT_PLUGIN2(smsdDriver, GammuSMSDDriver)
}
