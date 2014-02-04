#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <gammu.h>
#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include <QTimer>

namespace Gateway
{
namespace Driver
{
    class Device : public IDevice
    {
        Q_OBJECT
        public:
            Device(const DeviceInfo &info);
            ~Device();

            bool initialize();

            QString deviceId();
            NumberList phoneNumbers() const;

        private:
            void configure();
            bool connect();

        private:
            QString _serialPort;

            GSM_StateMachine *_stateMachine;
            GSM_Config *cfg;
            GSM_Error error;

            QTimer _timer;
            NumberList _numbers;
    };

}
}

#endif // DEVICE_HPP
