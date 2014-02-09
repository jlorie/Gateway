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

void Device::sendMessage(const IMessage *message)
{
    Q_UNUSED(message);
}
