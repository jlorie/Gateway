#include <include/DataStructures/MessageInfo.hpp>

#include "Device.hpp"
#include <signal.h>

namespace Gateway
{
namespace Driver
{
    const uint pollInterval = 1000;
    volatile GSM_Error sms_send_status;

    Device::Device(const DeviceInfo &info)
    {
        _serialPort = info.value(QString("serial_port"), QString("/dev/gsm_device"));
        _number = info.value("device_phonenumber");

        QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(checkForNewMessage()));
    }

    Device::~Device()
    {
        /* Terminate connection */
        error = GSM_TerminateConnection(_stateMachine);

        /* Free up used memory */
        GSM_FreeStateMachine(_stateMachine);
    }

    /* Interrupt signal handler */
    void interrupt(int sign)
    {
        signal(sign, SIG_IGN);
    }

    bool Device::initialize()
    {
        bool result(true);

        /* Register signal handler */
        signal(SIGINT, interrupt);
        signal(SIGTERM, interrupt);

        /*
         * We don't need gettext, but need to set locales so that
         * charset conversion works.
         */
        GSM_InitLocales(NULL);

        /* Allocates state machine */
        _stateMachine = GSM_AllocStateMachine();
        result = (_stateMachine != NULL);

        if (result)
        {
            configure();
            result = connect();

            _timer.start(pollInterval);
        }

        return result;
    }

    QString Device::deviceId()
    {
        char IMEI[30];
        GSM_GetIMEI(_stateMachine, IMEI);

        return QString(IMEI);
    }

    void Device::configure()
    {
        cfg = GSM_GetConfig(_stateMachine, 0);

        /*
         * Set configuration, first freeing old values.
         */
        free(cfg->Device);
        cfg->Device = strdup(_serialPort.toStdString().c_str());
        free(cfg->Connection);
        cfg->Connection = strdup("at");

        /* We have one valid configuration */
        GSM_SetConfigNum(_stateMachine, 1);

    }

    bool Device::connect()
    {
        GSM_Error error(GSM_InitConnection(_stateMachine, 1));
        bool result(ERR_NONE == error);

        if (!result)
            qWarning("Could not connet to device, %s", GSM_ErrorString(error));
        else
        {
            error = GSM_SetFastSMSSending(_stateMachine, TRUE);
            if (error != ERR_NONE)
                qDebug("Error setting fast sms sending, %s", GSM_ErrorString(error));
        }

        return result;
    }

    void send_sms_callback (GSM_StateMachine *sm, int status, int MessageReference, void * user_data)
    {
        Q_UNUSED(sm);
        Q_UNUSED(MessageReference);
        Q_UNUSED(user_data);

        if (status == 0)
        {
            sms_send_status = ERR_NONE;
        }
        else
        {
            sms_send_status = ERR_UNKNOWN;
        }
    }

    void Device::sendMessage(const IMessage *message)
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

        sms_send_status = ERR_TIMEOUT;

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

    void Device::checkForNewMessage()
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

    void Device::discardMessages()
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

                        sms.SMS[i].Folder = 0;
                        error = GSM_DeleteSMS(_stateMachine, &sms.SMS[i]);
                        switch (error)
                        {
                            case ERR_NONE:
                            case ERR_EMPTY:
                                break;
                            default:
                                qWarning("Error deleting SMS %s", GSM_ErrorString(error));
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
