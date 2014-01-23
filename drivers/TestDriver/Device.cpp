#include <include/CommonErrors.hpp>

#include "Device.hpp"

Device::Device(const DeviceInfo &info)
{
    Q_UNUSED(info);
}

Device::~Device()
{
}

qlonglong Device::deviceId()
{
    return 1;
}

NumberList Device::phoneNumbers() const
{
    return NumberList();
}
