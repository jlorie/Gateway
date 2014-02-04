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
            void sendMessage(const IMessage *message);

        protected slots:
            void checkForNewMessage();
            void discardMessages();

        private:
            void configure();
            bool connect();

        private:
            QString _serialPort;
            QString _number;

            GSM_StateMachine *_stateMachine;
            GSM_Config *cfg;
            GSM_Error error;
            bool _shutdown;

            QTimer _timer;
    };

}
}

#endif // DEVICE_HPP
