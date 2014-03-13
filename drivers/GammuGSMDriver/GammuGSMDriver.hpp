#ifndef GAMMUGSMDRIVER_HPP
#define GAMMUGSMDRIVER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/DriverInterface.hpp>

namespace Gateway
{
namespace Driver
{
    class GammuGSMDriver : public QObject, public DriverInterface
    {
        Q_OBJECT
        Q_INTERFACES(DriverInterface)

    public:
        IDevice *newDevice(const DeviceInfo &info);

        QString driverName() const;
        QString description() const;
    };
}
}
#endif // GAMMUGSMDRIVER_HPP
