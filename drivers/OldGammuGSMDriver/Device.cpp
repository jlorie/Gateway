#include <include/DataStructures/MessageInfo.hpp>

#include "Device.hpp"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <QDir>
#include <QMetaType>

namespace Gateway
{
namespace Driver
{
    const uint PollInterval = 1000;
    const uint GammuMaxErrors = 3;
    const uint GammuMaxResets = 3;

    volatile GSM_Error sms_send_status;

    Device::Device(const DeviceInfo &info)
        :_info(info)
    {
        _serialPort = _info.value(QString("serial_port"), QString("/dev/gsm_device"));
        _number = _info.value("device_phonenumber");

        _sending = false;
        _gammuErrors = 0;
        _gammuResets = 0;

        _isActive = false;

        qRegisterMetaType<MessageStatus>("MessageStatus");
        QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(checkForNewMessage()));
    }

    Device::~Device()
    {
        _timer.stop();
        terminateConnection();
    }

    bool Device::initialize()
    {
        bool result(true);

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

            if(result)
            {
                char IMEI[30];
                GSM_Error error = GSM_GetIMEI(_stateMachine, IMEI);
                if (error != ERR_NONE)
                {
                    qWarning("Error getting IMEI (%d): %s", error, GSM_ErrorString(error));
                    _gammuErrors++;
                }
                else
                {
                    _IMEI = QString(IMEI);
                }
            }
        }

        _isActive = result;
        return result;
    }

    QString Device::deviceIMEI()
    {
        return _IMEI;
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

    void Device::configure()
    {
        cfg = GSM_GetConfig(_stateMachine, 0);

        /*
         * Set configuration, first freeing old values.
         */
        free(cfg->Device);
        free(cfg->Connection);
        free(cfg->DebugFile);

        cfg->Device = strdup(qPrintable(_serialPort));
        cfg->Connection = strdup("at");

        if (_info.value("logs_enabled").toInt() != 0)
        {
            QString logFile(_info.value("log_file"));
            QDir logDir(logFile.mid(0, logFile.lastIndexOf("/")));

            if (logDir.exists())
            {
                cfg->DebugFile = strdup(qPrintable(logFile));
                strcpy(cfg->DebugLevel,"textalldate");
            }
            else
            {
                qWarning("Could not access to logs path \"%s\"", qPrintable(logDir.absolutePath()));
            }
        }

        /* We have one valid configuration */
        GSM_SetConfigNum(_stateMachine, 1);
    }

    bool Device::connect()
    {
        GSM_Error error(GSM_InitConnection(_stateMachine, 1));
        bool result(ERR_NONE == error);

        if (!result)
        {
            qWarning("Could not connect to device, %s", GSM_ErrorString(error));
            _gammuErrors++;
        }
        else
        {
            GSM_SetSendSMSStatusCallback(_stateMachine, send_sms_callback, NULL);

            PhoneSMSC.Location = 1;
            GSM_Error error = GSM_GetSMSC(_stateMachine, &PhoneSMSC);

            if (error != ERR_NONE)
            {
                qWarning("Error getting SMS Center (%d): %s", error, GSM_ErrorString(error));
                _gammuErrors++;
            }

            error = GSM_SetFastSMSSending(_stateMachine, TRUE);
            if (error != ERR_NONE)
            {
                qWarning("Error setting fast sms sending, %s", GSM_ErrorString(error));
                _gammuErrors++;
            }
        }

        //start polling
        _timer.start(PollInterval);

        return result;
    }

    void Device::sendMessage(const IMessage *message)
    {
        _messagesToSend.append(message);
    }

    MessageList Device::pendingMessages() const
    {
        MessageList messages;
        return messages;
    }

    void Device::checkForNewMessage()
    {
        if (_sending)
            return;

        if (_gammuErrors >= GammuMaxErrors) //Reset when more than 3 errors occurs
        {
            _timer.stop();
            if(_gammuResets >= GammuMaxResets)
            {
                emit connectionClosed();
                return;
            }

            qWarning("Resetting device with id %s", qPrintable(deviceIMEI()));

            GSM_Reset(_stateMachine, FALSE);
            _gammuErrors = 0;
            _gammuResets++;

            //Wait 5 seconds
            QTimer::singleShot(5 * 1000 /*5s*/, &_timer ,SLOT(start()));
        }

        GSM_SMSMemoryStatus	SMSStatus;
        GSM_Error error;
        GSM_MultiSMSMessage sms;

        bool new_message(false);

        error = GSM_GetSMSStatus(_stateMachine,&SMSStatus);
        if (error == ERR_NONE)
        {
            new_message = (SMSStatus.SIMUsed + SMSStatus.PhoneUsed > 0);
            _isActive = true;
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
            _isActive = true;
        }
        else
        {
            _isActive = false;

            qWarning("Error getting sms status (%d): %s", error, qPrintable(GSM_ErrorString(error)));
            _gammuErrors++;
        }

        if (new_message)
        {
            discardMessages();
        }

        if (_isActive)
        {
            while (!_messagesToSend.isEmpty())
            {
                const IMessage *message = _messagesToSend.takeFirst();
                sendGammuMessage(message);
            }
        }
    }

    void Device::discardMessages()
    {
        GSM_MultiSMSMessage sms;
        GSM_Error error = ERR_NONE;

        bool start(true);
        sms.Number = 0;
        sms.SMS[0].Location = 0;
        while (error == ERR_NONE)
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
                        if (!sms.SMS[i].InboxFolder)
                            continue;

                        handleMessage(sms.SMS[i]);

                        sms.SMS[i].Folder = 0;
                        error = GSM_DeleteSMS(_stateMachine, &sms.SMS[i]);
                        switch (error)
                        {
                            case ERR_NONE:
                            case ERR_EMPTY:
                                break;
                            default:
                            {
                                qWarning("Error deleting SMS (%d): %s", error, GSM_ErrorString(error));
                                _gammuErrors++;
                            }
                        }
                    }
                }
                    break;
                default:
                {
                    qWarning("Error getting SMS, (%d): %s", error, GSM_ErrorString(error));
                    _gammuErrors++;

                    return;
                }
            }
            start = false;
        }
    }

    void Device::discardMultiPart()
    {
        MultiPart *multiPart = qobject_cast<MultiPart *>(sender());

        foreach (MessageInfo *info, multiPart->parts())
        {
            emit newMessageReceived(info);
        }

        //remove from incompleteMessages
        uint messageId = multiPart->messageId();
        _incompleteMessages.take(messageId);

    }

    void Device::handleMessage(const GSM_SMSMessage &message)
    {
        // getting messageId
        int messageId;
        {
            messageId = message.UDH.ID8bit;
            if (messageId < 0)
                messageId = message.UDH.ID16bit;
        }

        MultiPart *multiPart(0);
        if (!_incompleteMessages.contains(messageId))
        {
             multiPart = new MultiPart(messageId, message, _number);
             _incompleteMessages.insert(messageId, multiPart);
             QObject::connect(multiPart, SIGNAL(timeout()), this, SLOT(discardMultiPart()));
        }
        else
        {
            multiPart = _incompleteMessages.value(messageId);
            multiPart->appendPart(message);
        }

        if (multiPart->isReady())
        {
            MessageInfo *info = multiPart->message();
            if (info)
                emit newMessageReceived(info);

            _incompleteMessages.take(messageId);
            delete multiPart;
        }
    }

    void Device::sendGammuMessage(const IMessage *message)
    {
        GSM_MultiSMSMessage SMS;
        GSM_MultiPartSMSInfo SMSInfo;
        unsigned char message_unicode[(message->body().length() + 1) * 2];

        GSM_ClearMultiPartSMSInfo(&SMSInfo);
        SMSInfo.Class = 1;
        SMSInfo.EntriesNum = 1;
        SMSInfo.UnicodeCoding = FALSE;
        SMSInfo.Entries[0].ID = SMS_ConcatenatedTextLong;
        EncodeUnicode(message_unicode, qPrintable(message->body()), message->body().length());
        SMSInfo.Entries[0].Buffer = message_unicode;

        error = GSM_EncodeMultiPartSMS(NULL, &SMSInfo, &SMS);
        if (error != ERR_NONE)
        {
            qWarning ("Error encoding MultiPart message (%d): %s", error, GSM_ErrorString(error));
            _gammuErrors++;

            return;
        }

        _sending = true;
        bool fail(false);
        for (int i = 0; i < SMS.Number && !fail; i++)
        {
            CopyUnicodeString(SMS.SMS[i].SMSC.Number, PhoneSMSC.Number);

            EncodeUnicode(SMS.SMS[i].Number, qPrintable(message->to()), message->to().length());
            SMS.SMS[i].PDU = SMS_Submit;

            sms_send_status = ERR_TIMEOUT;

            error = GSM_SendSMS(_stateMachine, &SMS.SMS[i]);
            if (error != ERR_NONE)
            {
                qWarning("Error sending message (%d) %s", error, GSM_ErrorString(error));
                _gammuErrors++;

                return;
            }

            /* Wait for network reply */
            forever
            {
                GSM_ReadDevice(_stateMachine, TRUE);
                if (sms_send_status == ERR_NONE)
                {
                    if ((i + 1) == SMS.Number)
                    {
                        emit messageStatusChanged(message->id(), stSent);
                    }

                    break;
                }
                if (sms_send_status != ERR_TIMEOUT)
                {
                    emit messageStatusChanged(message->id(), stFailed);
                    fail = true;
                }
            }
        }
        _sending = false;
    }

    void Device::terminateConnection()
    {
        error = GSM_TerminateConnection(_stateMachine);
        if (error != ERR_NONE)
        {
            qWarning("Error terminating connection (%d): %s", error, GSM_ErrorString(error));
        }

        /* Free up used memory */
        GSM_FreeStateMachine(_stateMachine);
    }

}
}
