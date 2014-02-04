#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        QString deviceId();
        void sendMessage(const IMessage *message);
};

#endif // DEVICE_HPP
