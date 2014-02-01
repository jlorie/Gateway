#ifndef IPHONENUMBER
#define IPHONENUMBER

#include "IMessage.hpp"
#include "DataStructures/MessageList.hpp"

#include <QObject>
#include <QString>

class IPhoneNumber: public QObject
{
    Q_OBJECT
    public:
        virtual ~IPhoneNumber() {}

        virtual QString number() const = 0;
        virtual void sendMessage(const IMessage *message) = 0;

    signals:
        void newMessageReceived(const IMessage *message);
        void messageStatusChanged(qlonglong messageId, MessageStatus status);
};


#endif // IPHONENUMBER_HPP
