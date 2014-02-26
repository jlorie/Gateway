#ifndef MESSAGEBUILDER_HPP
#define MESSAGEBUILDER_HPP

#include <include/DataStructures/MessageInfo.hpp>
#include <gammu-message.h>

#include <QObject>
#include <QVector>
#include <QString>

namespace Gateway
{
namespace Driver
{
    typedef QVector<QString> MessageParts;
    typedef QList<MessageInfo *> MessageInfoList;

    class MultiPart : public QObject
    {
        Q_OBJECT
    public:
        MultiPart(const uint messageId, const GSM_SMSMessage &gammuMessage, const QString &toNumber);

        uint messageId() const;
        void appendPart(const GSM_SMSMessage &message);
        bool isReady() const;

        MessageInfo *message() const;
        MessageInfoList parts() const;

    signals:
        void timeout();

    private slots:
        void onTimeOut();

    private:
        uint _messageId;
        MessageParts _bodys;
        MessageInfo *_message;
        GSM_SMSMessage _gammuMessage;
    };
}
}

#endif // MESSAGEBUILDER_HPP
