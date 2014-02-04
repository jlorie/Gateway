#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>
#include <physical/SerialPhysicalLayer.hpp>
#include <protocol/ATMessage.hpp>

class Device : public IDevice
{
    Q_OBJECT
    public:
        Device(const DeviceInfo &info);
        bool initialize();

        QString deviceId();
        void sendMessage(const IMessage *message);

    public slots:
        void messageReceived(const QString &frame);

    private:
        QString normalizeNumber(const QString &number);

    private:
        QString _serialPort;
        QString _number;

        ATMessage _message;
        SerialPhysicalLayer *_physical;
};

#endif // DEVICE_HPP
