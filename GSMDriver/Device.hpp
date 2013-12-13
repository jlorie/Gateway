#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <QObject>
#include <QtPlugin>

#include <include/IDevice.hpp>
#include <physical/SerialPhysicalLayer.hpp>
#include <protocol/ATMessage.hpp>

class Device : public QObject, public IDevice
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Gateway.IDevice")
    Q_INTERFACES(IDevice)

    public:
        Device();
        ulong initialize(const DeviceInfo &info);
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
