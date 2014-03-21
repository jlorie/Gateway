#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include "wrapper/GammuSMSD.hpp"

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

            QString deviceIMEI();
            void sendMessage(const IMessage *message);
            MessageList pendingMessages() const;

        protected slots:
            void onMessageReceived(const QString &from, const QString &body);

        private:
            const DeviceInfo _info;
            GammuSMSD *_smsd;
            QString _number;
    };

}
}

#endif // DEVICE_HPP
