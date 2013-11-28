#include "AbstractGSMDevice.hpp"

namespace Gateway
{
    AbstractGSMDevice::AbstractGSMDevice(const DeviceInfo &info, QObject *parent) :
        QObject(parent), _deviceInfo(info)
    {
    }

    AbstractGSMDevice::~AbstractGSMDevice()
    {
    }

    bool AbstractGSMDevice::initialize()
    {
        return true;
    }

    DeviceInfo AbstractGSMDevice::info() const
    {
        return _deviceInfo;
    }
}
