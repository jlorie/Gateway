#include "MultiPart.hpp"
#include <gammu.h>

#include <QTimer>

namespace Gateway
{
namespace Driver
{
    const uint TimeOut = 10 * 1000; // 10s

    MultiPart::MultiPart(const uint messageId, const GSM_SMSMessage &gammuMessage, const QString &toNumber)
        :_messageId(messageId), _gammuMessage(gammuMessage)
    {
        _message = new MessageInfo(QString(DecodeUnicodeConsole(gammuMessage.Number)),
                                   toNumber, QString());

        // getting size
        int size;
        {
            size = gammuMessage.UDH.AllParts;
            if (size <= 0)
                size = 1;
        }

        QTimer::singleShot(TimeOut, this, SLOT(onTimeOut()));

        _bodys.resize(size);
        appendPart(gammuMessage);
    }

    uint MultiPart::messageId() const
    {
        return _messageId;
    }

    void MultiPart::appendPart(const GSM_SMSMessage &message)
    {
        int index;
        {
            index = message.UDH.PartNumber;
            if (index > 0)
                index --;
            else
                index = 0;
        }

        _bodys[index]= QString(DecodeUnicodeConsole(message.Text));
    }

    bool MultiPart::isReady() const
    {
        bool result(true);

        foreach (QString part, _bodys)
        {
            if (part.isEmpty())
                result = false;
        }

        return result;
    }

    MessageInfo *MultiPart::message() const
    {
        MessageInfo *result = 0;
        QString body;

        foreach (QString part, _bodys)
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

    MessageInfoList MultiPart::parts() const
    {
        MessageInfoList list;

        foreach (QString part, _bodys)
        {
            if (part.isEmpty())
                continue;

            list.append(new MessageInfo(_message->from(),
                                        _message->to(),
                                        part, _message->id()));
        }

        return list;
    }

    void MultiPart::onTimeOut()
    {
        if (!isReady())
            emit timeout();
    }

}
}
