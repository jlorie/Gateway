#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        ~Device();

        qlonglong deviceId();
        NumberList phoneNumbers() const;
        DeviceClass type() const;
};

#endif // DEVICE_HPP
