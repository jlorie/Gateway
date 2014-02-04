#include <include/CommonErrors.hpp>

#include "Device.hpp"

Device::Device(const DeviceInfo &info)
{
    Q_UNUSED(info);
}

Device::~Device()
{
}

QString Device::deviceId()
{
    return QString("0000000000");
}

NumberList Device::phoneNumbers() const
{
    return NumberList();
}
