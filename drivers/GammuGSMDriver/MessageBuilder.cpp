#include "MessageBuilder.hpp"
#include <gammu.h>

namespace Gateway
{
namespace Driver
{
    MessageBuilder::MessageBuilder(const GSM_SMSMessage &gammuMessage, const QString &toNumber)
        :_gammuMessage(gammuMessage)
    {
        _message = new MessageInfo(QString(DecodeUnicodeConsole(gammuMessage.Number)),
                                   toNumber, QString());

        // getting size
        int size;
        {
            size = gammuMessage.UDH.AllParts;
            if (size < 0)
                size = 1;
        }

        _parts.resize(size);
        appendPart(gammuMessage);
    }

    void MessageBuilder::appendPart(const GSM_SMSMessage &message)
    {
        int index;
        {
            index = message.UDH.PartNumber;
            if (index > 0)
                index --;
        }

        _parts[index]= QString(DecodeUnicodeConsole(message.Text));
    }

    bool MessageBuilder::isReady() const
    {
        bool result(true);

        foreach (QString part, _parts)
        {
            if (part.isEmpty())
                result = false;
        }

        return result;
    }

    MessageInfo *MessageBuilder::message() const
    {
        MessageInfo *result = 0;
        QString body;

        foreach (QString part, _parts)
        {
            if (part.isEmpty())
            {
                body.clear();
                break;
            }

            body.append(part);
        }

        if (!body.isEmpty())
        {
            _message->setBody(body);
            result = _message;
        }

        return result;
    }

}
}
