#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <gammu.h>
#include <include/IDevice.hpp>
#include <include/DataStructures/DeviceInfo.hpp>

#include "MultiPart.hpp"

#include <QTimer>

namespace Gateway
{
namespace Driver
{
    typedef QMap<int, MultiPart *> MultiPartMap;

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
            void checkForNewMessage();
            void discardMessages();
            void discardMultiPart();

        private:
            void configure();
            bool connect();
            void handleMessage(const GSM_SMSMessage &message);
            void sendGammuMessage(const IMessage *message);

            void terminateConnection();

        private:
            const DeviceInfo _info;
            QString _serialPort;
            QString _number;

            MultiPartMap _incompleteMessages;
            MessageList _messagesToSend;

            GSM_StateMachine *_stateMachine;
            GSM_Config *cfg;
            GSM_Error error;

            uint _gammuErrors;
            uint _gammuResets;

            bool _sending;
            QTimer _timer;
            GSM_SMSC PhoneSMSC;
            QString _IMEI;

            bool _isActive;
    };

}
}

#endif // DEVICE_HPP
