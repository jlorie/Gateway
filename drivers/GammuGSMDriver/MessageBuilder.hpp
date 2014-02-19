#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP

#include <include/DataStructures/MessageInfo.hpp>
#include <gammu-message.h>

#include <QVector>
#include <QString>

namespace Gateway
{
namespace Driver
{
    typedef QVector<QString> MessageParts;

    class MessageBuilder
    {
    public:
        MessageBuilder(const GSM_SMSMessage &gammuMessage, const QString &toNumber);

        void appendPart(const GSM_SMSMessage &message);
        bool isReady() const;

        MessageInfo *message() const;

    private:
        MessageParts _parts;
        MessageInfo *_message;
        GSM_SMSMessage _gammuMessage;
    };
}
}

#endif // MESSAGEBUILDER_HPP
