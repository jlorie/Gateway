#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DeviceInfo.hpp>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        qlonglong deviceId();
        NumberList phoneNumbers() const;
};

#endif // DEVICE_HPP
