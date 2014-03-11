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

            QString deviceId();
            void sendMessage(const IMessage *message);
            void onMessageSent(const int &MessageReference, const MessageStatus &status);

        protected slots:
            void checkForNewMessage();
            void discardMessages();
            void discardMultiPart();

        private:
            void configure();
            bool connect();
            void sendMessageFromQueue();
            void handleMessage(const GSM_SMSMessage &message);

        private:
            const DeviceInfo _info;
            QString _serialPort;
            QString _number;

            MultiPartMap _incompleteMessages;

            GSM_StateMachine *_stateMachine;
            GSM_Config *cfg;
            GSM_Error error;

            bool _sending;
            QTimer _timer;
            GSM_SMSC PhoneSMSC;
    };

}
}

#endif // DEVICE_HPP
