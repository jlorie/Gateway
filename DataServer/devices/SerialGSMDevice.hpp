#ifndef SERIALGSMDEVICE_HPP
#define SERIALGSMDEVICE_HPP

#include "AbstractGSMDevice.hpp"
#include "protocol/ATMessage.hpp"
#include "physical/SerialPhysicalLayer.hpp"

namespace Gateway
{
    class SerialGSMDevice : public AbstractGSMDevice
    {
        Q_OBJECT
    public:
        SerialGSMDevice(const DeviceInfo &info, QObject *parent = 0);

        virtual bool initialize();        

    public slots:
        void messageReceived(const QString &frame);

    private:
        SerialPhysicalLayer *_physical;
        ATMessage _message;

    };
}
#endif // SERIALGSMDEVICE_HPP
