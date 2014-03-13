#ifndef TESTDRIVER_HPP
#define TESTDRIVER_HPP

#include <QtPlugin>
#include <QObject>

#include <include/DriverInterface.hpp>
#include "Device.hpp"

namespace Gateway
{
    class GammuSMSDDriver : public QObject, public DriverInterface
    {
        Q_OBJECT
        Q_INTERFACES(DriverInterface)

        IDevice *newDevice(const DeviceInfo &info);

        QString driverName() const;
        QString description() const;
    };
}
#endif // TESTDRIVER_HPP
