#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <physical/SerialPhysicalLayer.hpp>
#include <protocol/ATMessage.hpp>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        bool initialize();

        qlonglong deviceId();

        NumberList phoneNumbers() const;

        DeviceClass type() const;

    public slots:
        void messageReceived(const QString &frame);

    private:
        NumberList _numbers;
        QString _serialPort;

        ATMessage _message;
        SerialPhysicalLayer *_physical;
};

#endif // DEVICE_HPP
