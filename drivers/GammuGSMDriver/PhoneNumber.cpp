#include "PhoneNumber.hpp"
#include <include/DataStructures/MessageInfo.hpp>

namespace Gateway
{
namespace Driver
{
    const uint pollInterval = 1000;
    volatile GSM_Error sms_send_status;

    PhoneNumber::PhoneNumber(const QString &number, GSM_StateMachine *stateMachine)
        :_number(number), _stateMachine(stateMachine)
    {
        connect(&_timer, SIGNAL(timeout()), this, SLOT(checkForNewMessage()));
        _timer.start(pollInterval);
    }

    PhoneNumber::~PhoneNumber()
    {
    }

    QString PhoneNumber::number() const
    {
        return _number;
    }

    void send_sms_callback (GSM_StateMachine *sm, int status, int MessageReference, void * user_data)
    {
        Q_UNUSED(sm);
        Q_UNUSED(MessageReference);
        Q_UNUSED(user_data);

        if (status == 0)
            sms_send_status = ERR_NONE;
        else
            sms_send_status = ERR_UNKNOWN;
    }

    void PhoneNumber::sendMessage(const IMessage *message)
    {
        GSM_SMSMessage sms;
        GSM_SMSC PhoneSMSC;

        /* Cleanup the structure */
        memset(&sms, 0, sizeof(sms));
        EncodeUnicode(sms.Text, message->body().toStdString().c_str(), message->body().length());
        EncodeUnicode(sms.Number, message->to().toStdString().c_str(), message->to().length());

        /* We want to submit message */
        sms.PDU = SMS_Submit;
        /* No UDH, just a plain message */
        sms.UDH.Type = UDH_NoUDH;
        /* We used default coding for text */
        sms.Coding = SMS_Coding_Default_No_Compression;
        /* Class 1 message (normal) */
        sms.Class = 1;

        /* Set callback for message sending */
        /* This needs to be done after initiating connection */
        GSM_SetSendSMSStatusCallback(_stateMachine, send_sms_callback, NULL);

        /* We need to know SMSC number */
        PhoneSMSC.Location = 1;
        GSM_Error error = GSM_GetSMSC(_stateMachine, &PhoneSMSC);
        /* Set SMSC number in message */
        CopyUnicodeString(sms.SMSC.Number, PhoneSMSC.Number);

        /* Send message */
        error = GSM_SendSMS(_stateMachine, &sms);
        if (error != ERR_NONE)
        {
            emit messageStatusChanged(message->id(), stFailed);
            qWarning("Error sending message, %s", GSM_ErrorString(error));
        }

        /* Wait for network reply */
        while (!_shutdown)
        {
            GSM_ReadDevice(_stateMachine, TRUE);
            if (sms_send_status == ERR_NONE)
            {
                emit messageStatusChanged(message->id(), stSent);
                break;
            }
            if (sms_send_status != ERR_TIMEOUT)
            {
                emit messageStatusChanged(message->id(), stFailed);
                break;
            }
        }
    }

    void PhoneNumber::checkForNewMessage()
    {
        GSM_SMSMemoryStatus	SMSStatus;
        GSM_Error error;
        GSM_MultiSMSMessage sms;

        bool new_message(false);

        error = GSM_GetSMSStatus(_stateMachine,&SMSStatus);
        if (error == ERR_NONE)
        {
            new_message = (SMSStatus.SIMUsed + SMSStatus.PhoneUsed > 0);
        }
        else
        if (error == ERR_NOTSUPPORTED || error == ERR_NOTIMPLEMENTED)
        {
            /* Fallback to GetNext */
            sms.Number = 0;
            sms.SMS[0].Location = 0;
            sms.SMS[0].Folder = 0;
            error = GSM_GetNextSMS(_stateMachine, &sms, TRUE);
            new_message = (error == ERR_NONE);
        }

        if (new_message)
        {
            discardMessages();
        }
    }

    void PhoneNumber::discardMessages()
    {
        GSM_MultiSMSMessage sms;
        GSM_Error error = ERR_NONE;

        bool start(true);
        sms.Number = 0;
        sms.SMS[0].Location = 0;
        while (error == ERR_NONE /*&& !Config->shutdown*/)
        {
            sms.SMS[0].Folder = 0;
            error = GSM_GetNextSMS(_stateMachine, &sms, start);
            switch (error)
            {
                case ERR_EMPTY:
                    break;
                case ERR_NONE:
                {
                    for (int i = 0; i < sms.Number; i++)
                    {
                        emit newMessageReceived(
                                    new MessageInfo(QString(DecodeUnicodeConsole(sms.SMS[i].Number)),_number,
                                                    QString(DecodeUnicodeConsole(sms.SMS[i].Text))));

                        error = GSM_DeleteSMS(_stateMachine, &sms.SMS[i]);
                        switch (error)
                        {
                            case ERR_NONE:
                            case ERR_EMPTY:
                                break;
                            default:
                                qWarning("Error deleting SMS %d", error);
                        }
                    }
                }
                    break;
                default:
                    qWarning("Error getting SMS, %d", error);
                    return;
            }
            start = false;
        }
    }
}
}
