#include "GammuSMSD.hpp"

#include <QMetaType>

namespace Gateway
{
namespace Driver
{

    const int MaxFailures = 3;
    GSM_Error _configSendingSMSStatus;

    GammuSMSD::GammuSMSD(const QString &serialPort)
        :QThread(), _serialPort(serialPort)
    {
        _configCommTimeout = 2;
        _configSendTimeout = 10;
        _configReceiveFrequency = 0;
        _configLoopSleep = 1;
        _configResetFrequency = 0;
        _configHardResetFrequency = 0;
        _configMultipartTimeout = 600;
        _configShutdown = false;
        _configRelativeValidity = -1;
        _configCurrDeliveryReport = 0;

        PhoneSMSC.Location = -1;
        _SMSCCache.Location = 0;

        /*
         * We don't need gettext, but need to set locales so that
         * charset conversion works.
         */
        GSM_InitLocales(NULL);

        /* Allocates state machine */
        _stateMachine = GSM_AllocStateMachine();
        if (_stateMachine == NULL)
        {
            qWarning("Error creating state machine");
            return;
        }

        cfg = GSM_GetConfig(_stateMachine, 0);

        /*
         * Set configuration, first freeing old values.
         */
        free(cfg->Device);
        free(cfg->Connection);
        free(cfg->DebugFile);

        cfg->Device = strdup(qPrintable(_serialPort));
        cfg->Connection = strdup("at");

        /* We have one valid configuration */
        GSM_SetConfigNum(_stateMachine, 1);

        _configIncompleteMessageID = -1;
        _configIncompleteMessageTime = 0;

        qRegisterMetaType<MessageStatus>("MessageStatus");
        connect(this, SIGNAL(messageStatusChanged(qlonglong,MessageStatus)),
                this, SLOT(onMessageStatusChanged(qlonglong,MessageStatus)));
    }

    GammuSMSD::~GammuSMSD()
    {
        stop();
    }

    /**
     * Callback from libGammu on sending message.
     */
    void SMSD_SendSMSStatusCallback (GSM_StateMachine *sm, int status, int mr, void *user_data)
    {
        Q_UNUSED(sm);
        Q_UNUSED(mr);
        Q_UNUSED(user_data);

        /* Was message sent okay? */
        if (status == 0)
        {
            _configSendingSMSStatus = ERR_NONE;
        } else
        {
            _configSendingSMSStatus = ERR_UNKNOWN;
        }
    }

    bool GammuSMSD::init()
    {
        bool result(true);
        GSM_Error error;

        qDebug("Starting phone communication (%s) ...", qPrintable(_serialPort));
        error = GSM_InitConnection(_stateMachine, 1);
        result = (error == ERR_NONE);

        if (result)
        {
            GSM_SetSendSMSStatusCallback(_stateMachine, SMSD_SendSMSStatusCallback, NULL);

            PhoneSMSC.Location = 1;
            if (GSM_GetSMSC(_stateMachine, &PhoneSMSC) == ERR_NONE)
            {
                error = GSM_GetIMEI(_stateMachine, _configStatusIMEI);
                if (error != ERR_NONE)
                {
                    qWarning("Error getting IMEI (%d): %s", error, GSM_ErrorString(error));
                    result = false;
                }

                GSM_SetFastSMSSending(_stateMachine, TRUE);
            }
            else
            {
                result = false;
            }

        }

        return result;
    }

    void GammuSMSD::run()
    {
        mainLoop();
    }

    void GammuSMSD::stop()
    {
        SMSD_Terminate(ERR_NONE);
    }

    void GammuSMSD::injectSms(const QString &to, const QString &body, qlonglong id)
    {
        GSM_Error error;
        GSM_MultiSMSMessage SMS;
        GSM_MultiPartSMSInfo SMSInfo;
        unsigned char message_unicode[(body.length() + 1) * 2];

        GSM_ClearMultiPartSMSInfo(&SMSInfo);
        SMSInfo.Class = 1;
        SMSInfo.EntriesNum = 1;
        SMSInfo.UnicodeCoding = FALSE;
        SMSInfo.Entries[0].ID = SMS_ConcatenatedTextLong;
        EncodeUnicode(message_unicode, qPrintable(body), body.length());
        SMSInfo.Entries[0].Buffer = message_unicode;

        error = GSM_EncodeMultiPartSMS(NULL, &SMSInfo, &SMS);
        if (error != ERR_NONE)
        {
            qWarning ("Error encoding MultiPart message (%d): %s", error, GSM_ErrorString(error));
            return;
        }

        for (int i = 0; i < SMS.Number; i++)
        {
            CopyUnicodeString(SMS.SMS[i].SMSC.Number, PhoneSMSC.Number);

            EncodeUnicode(SMS.SMS[i].Number, qPrintable(to), to.length());
            SMS.SMS[i].PDU = SMS_Submit;

            _mutex.lock();
            _smsToSend.append(SmsData(id, SMS.SMS[i]));
            _mutex.unlock();
        }
    }

    void GammuSMSD::handleMessage(GSM_MultiSMSMessage *sms)
    {
        QString from;
        QString body;

        for (int i = 0; i < sms->Number; i++)
        {
            from = QString(DecodeUnicodeConsole(sms->SMS[i].Number));
            body += QString(DecodeUnicodeConsole(sms->SMS[i].Text));
        }

        emit messageReceived(from, body);
    }

    bool GammuSMSD::sendQueuedMessages()
    {
        bool result(false);

        GSM_DateTime         	Date;
        GSM_Error            	error;
        unsigned int         	j;
        int	 z;

        if (_configShutdown)
        {
            return false;
        }

        for (int i = 0; i < _smsToSend.size(); ++i)
        {
            _mutex.lock();
            qlonglong messageId = _smsToSend.at(i).first;
            GSM_SMSMessage &message = const_cast<GSM_SMSMessage&>(_smsToSend.at(i).second); //FIXME
            _mutex.unlock();


            if (message.SMSC.Location == 0 && UnicodeLength(message.SMSC.Number) == 0 && PhoneSMSC.Location == 0)
            {
                qDebug("Message without SMSC, using configured one");

                memcpy(&message.SMSC,&PhoneSMSC,sizeof(GSM_SMSC));
                message.SMSC.Location = 0;
                if (_configRelativeValidity != -1)
                {
                    message.SMSC.Validity.Format	  = SMS_Validity_RelativeFormat;
                    message.SMSC.Validity.Relative = (GSM_ValidityPeriod)_configRelativeValidity;
                }

            }

            if (message.SMSC.Location == 0 && UnicodeLength(message.SMSC.Number) == 0)
            {
                qDebug("Message without SMSC, assuming you want to use the one from phone");
                message.SMSC.Location = 1;
            }

            if (message.SMSC.Location != 0)
            {
                if (_SMSCCache.Location != message.SMSC.Location)
                {
                    _SMSCCache.Location = message.SMSC.Location;
                    error = GSM_GetSMSC(_stateMachine, &_SMSCCache);

                    if (error!=ERR_NONE)
                    {
                        qWarning("Error getting SMSC from phone (%d): %s", error, GSM_ErrorString(error));

                        emit messageStatusChanged(messageId, stFailed);
                        return result;
                    }
                }

                memcpy(&message.SMSC,&_SMSCCache,sizeof(GSM_SMSC));
                message.SMSC.Location = 0;
                if (_configRelativeValidity != -1)
                {
                    message.SMSC.Validity.Format	  = SMS_Validity_RelativeFormat;
                    message.SMSC.Validity.Relative = (GSM_ValidityPeriod)_configRelativeValidity;
                }
            }

            if (_configCurrDeliveryReport == 1)
            {
                message.PDU = SMS_Status_Report;
            }

            _configSendingSMSStatus = ERR_TIMEOUT;
            error = GSM_SendSMS(_stateMachine, &message);
            if (error != ERR_NONE)
            {
                qWarning("Error sending SMS (%d): %s", error, GSM_ErrorString(error));
                emit messageStatusChanged(messageId, stFailed);

                return result;
            }

            j = 0;
            while (!_configShutdown)
            {
                GSM_GetCurrentDateTime(&Date);
                z = Date.Second;
                while (z == Date.Second)
                {
                    usleep(10000);
                    GSM_GetCurrentDateTime(&Date);
                    GSM_ReadDevice(_stateMachine, TRUE);
                    if (_configSendingSMSStatus != ERR_TIMEOUT)
                    {
                        break;
                    }
                }
                if (_configSendingSMSStatus != ERR_TIMEOUT)
                {
                    break;
                }
                j++;
                if (j > _configSendTimeout)
                {
                    break;
                }
            }

            if (_configSendingSMSStatus != ERR_NONE)
            {
                qWarning("Error getting send status of message (%d): %s", error, GSM_ErrorString(error));
                emit messageStatusChanged(messageId, stFailed);
            }
            else
            {
                qDebug("Message sent successfully");
                emit messageStatusChanged(messageId, stSent);

                result = true;
            }
        }

        return result;
    }

    void GammuSMSD::onMessageStatusChanged(const qlonglong &id, const MessageStatus status)
    {
        Q_UNUSED(status);

        _mutex.lock();
        {
            for (int i = 0; i < _smsToSend.size(); ++i)
            {
                if (_smsToSend.at(i).first == id)
                {
                    _smsToSend.removeAt(i);
                    break;
                }
            }

            _mutex.unlock();
        }
    }

    QString GammuSMSD::imsi() const
    {
        return QString(_configStatusIMEI);
    }

    void GammuSMSD::mainLoop()
    {
        GSM_Error error;
        int errors = -1, initerrors=0;
        time_t  lastreceive = 0, lastreset = time(NULL), lasthardreset = time(NULL), lastnothingsent = 0;
        time_t	lastloop = 0, current_time;
        int i;
        gboolean force_reset = FALSE, force_hard_reset = FALSE;

        while (!_configShutdown)
        {
            lastloop = time(NULL);
            /* There were errors in communication - try to recover */
            if (errors > 2 || force_reset || force_hard_reset)
            {
                /* Should we disconnect from phone? */
                if (GSM_IsConnected(_stateMachine))
                {
                    if (! force_reset && ! force_hard_reset)
                    {
                        qDebug("Already hit %d errors", errors);
                    }
                    qWarning("Terminating communication");
                    GSM_TerminateConnection(_stateMachine);
                }
                /* Did we reach limit for errors? */
                if (MaxFailures != 0 && initerrors > MaxFailures)
                {
                    qWarning("Reached maximal number of failures (%d), terminating", MaxFailures);
                    emit connectionTerminated();

                    break;
                }
                if (initerrors++ > 3)
                {
                    qDebug("Going to 30 seconds sleep because of too much connection errors");

                    for (i = 0; i < 60; i++)
                    {
                        if (_configShutdown)
                            break;
                        usleep(500000);
                    }
                }
                qDebug("Starting phone communication...");
                error = GSM_InitConnection(_stateMachine, 1);
                /* run on error */
                if (error != ERR_NONE)
                {
                    qWarning("Connection error (%d): %s", error, GSM_ErrorString(error));
                }
                switch (error)
                {
                case ERR_NONE:
                    GSM_SetSendSMSStatusCallback(_stateMachine, SMSD_SendSMSStatusCallback, NULL);

                    if (initerrors > 3 || force_reset )
                    {
                        error = GSM_Reset(_stateMachine, FALSE); /* soft reset */
                        qDebug("Soft reset return code");

                        lastreset = time(NULL);
                        sleep(5);
                        force_reset = FALSE;
                    }
                    if (force_hard_reset)
                    {
                        error = GSM_Reset(_stateMachine, TRUE); /* hard reset */
                        qWarning("Hard reset return code");

                        lasthardreset = time(NULL);
                        sleep(5);
                        force_hard_reset = FALSE;
                    }
                    break;
                case ERR_DEVICEOPENERROR:
                    SMSD_Terminate(error);
                    return;
                default:
                    qWarning("Error at init connection (%d): %s", error, GSM_ErrorString(error));
                    errors = 250;
                    break;
                }
                continue;
            }

            /* Should we receive? */
            if (((difftime(time(NULL), lastreceive) >= _configReceiveFrequency) || (_configSendingSMSStatus != ERR_NONE)))
            {
                lastreceive = time(NULL);
                initerrors = 0;

                /* read all incoming SMS */
                if (!SMSD_CheckSMSStatus())
                {
                    errors++;
                    continue;
                }
                else
                {
                    errors = 0;
                }

            }

            /* time for preventive reset */
            current_time = time(NULL);
            if (_configResetFrequency > 0 && difftime(current_time, lastreset) >= _configResetFrequency)
            {
                force_reset = true;
                continue;
            }
            if (_configHardResetFrequency > 0 && difftime(current_time, lasthardreset) >= _configHardResetFrequency)
            {
                force_hard_reset = true;
                continue;
            }

            /* Send any queued messages*/
            current_time = time(NULL);
            if ((difftime(current_time, lastnothingsent) >= _configCommTimeout))
            {
                if (!sendQueuedMessages()) // nada por enviar
                {
                    lastnothingsent = current_time;
                }
                /* We don't care about other errors here, they are handled in SMSD_SendSMS */
            }

            /* Sleep some time before another loop */
            current_time = time(NULL);
            if (_configLoopSleep == 1)
            {
                sleep(1);
            }
            else
            if (difftime(current_time, lastloop) < _configLoopSleep)
            {
                sleep(_configLoopSleep - difftime(current_time, lastloop));
            }
        }
    }


    /**
     * Terminates SMSD, closing phone connection, closing log files and
     * possibly reporting error code.
     */
    void GammuSMSD::SMSD_Terminate(GSM_Error error)
    {
        GSM_Error ret = ERR_NONE;

        _configShutdown = true;

        /* Log error message */
        if (error != ERR_NONE && error != 0)
        {
            qCritical("Critical error (%d): %s", error, GSM_ErrorString(error));
        }

        /* Disconnect from phone */
        if (GSM_IsConnected(_stateMachine))
        {
            qDebug("Terminating communication...");
            ret = GSM_TerminateConnection(_stateMachine);

            if (ret != ERR_NONE)
            {
                printf("%s\n", GSM_ErrorString(error));
                /* Try again without checking errors */
                if (GSM_IsConnected(_stateMachine))
                {
                    qDebug("Terminating communication for second time...");
                    GSM_TerminateConnection(_stateMachine);
                }
            }
        }

        /* Should we terminate program? */ // TODO
    }

    /**
     * Checks whether there are some messages to process and calls
     * SMSD_ReadDeleteSMS to process them.
     */
    bool GammuSMSD::SMSD_CheckSMSStatus()
    {
        GSM_SMSMemoryStatus	SMSStatus;
        GSM_Error		error;
        bool new_message(false);
        GSM_MultiSMSMessage sms;

        /* Do we have any SMS in phone ? */

        /* First try SMS status */
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
        else
        {
            qWarning("Error getting SMS status (%d): %s", error, GSM_ErrorString(error));
            return FALSE;
        }

        /* Yes. We have SMS in phone */
        if (new_message)
        {
            return SMSD_ReadDeleteSMS();
        }

        return TRUE;
    }

    /**
     * Reads message from phone, processes it and delete it from phone afterwards.
     *
     * It tries to link multipart messages together if possible.
     */
    bool GammuSMSD::SMSD_ReadDeleteSMS()
    {
        bool start;
        GSM_MultiSMSMessage sms;
        GSM_MultiSMSMessage **GetSMSData = NULL, **SortedSMS;
        int allocated = 0;
        GSM_Error error = ERR_NONE;
        int GetSMSNumber = 0;
        int i, j;

        /* Read messages from phone */
        start=TRUE;
        sms.Number = 0;
        sms.SMS[0].Location = 0;
        while (error == ERR_NONE && !_configShutdown)
        {
            sms.SMS[0].Folder = 0;
            error = GSM_GetNextSMS(_stateMachine, &sms, start);

            switch (error)
            {
                case ERR_EMPTY:
                    break;
                case ERR_NONE:
                    {
                        if (allocated <= GetSMSNumber + 2)
                        {
                            GetSMSData = (GSM_MultiSMSMessage **)realloc(GetSMSData, (allocated + 20) * sizeof(GSM_MultiSMSMessage *));
                            if (GetSMSData == NULL)
                            {
                                qWarning("Failed to allocate memory");
                                return false;
                            }
                            allocated += 20;
                        }
                        GetSMSData[GetSMSNumber] = (GSM_MultiSMSMessage *)malloc(sizeof(GSM_MultiSMSMessage));

                        if (GetSMSData[GetSMSNumber] == NULL)
                        {
                            qWarning("Failed to allocate memory");
                            return false;
                        }

                        *(GetSMSData[GetSMSNumber]) = sms;
                        GetSMSNumber++;
                        GetSMSData[GetSMSNumber] = NULL;
                    }
                    break;
                default:
                    qWarning("Error getting SMS (%d): %s", error, GSM_ErrorString(error));
                    return false;
            }
            start = false;
        }

        /* Log how many messages were read */
        qDebug("Read %d messages", GetSMSNumber);

        /* No messages to process */
        if (GetSMSNumber == 0)
        {
            return true;
        }

        /* Allocate memory for sorted messages */
        SortedSMS = (GSM_MultiSMSMessage **)malloc(allocated * sizeof(GSM_MultiSMSMessage *));
        if (SortedSMS == NULL)
        {
            qWarning("Failed to allocate memory for linking messages");
            qWarning("Skipping linking messages, long messages will not be connected");
            SortedSMS = GetSMSData;
        }
        else
        {
            /* Link messages */
            error = GSM_LinkSMS(GSM_GetDebug(_stateMachine), GetSMSData, SortedSMS, TRUE);
            if (error != ERR_NONE) return FALSE;

            /* Free memory */
            for (i = 0; GetSMSData[i] != NULL; i++) {
                free(GetSMSData[i]);
                GetSMSData[i] = NULL;
            }
            free(GetSMSData);
        }

        /* Process messages */
        for (i = 0; SortedSMS[i] != NULL; i++)
        {
            /* Check multipart message parts */
            if (!SMSD_CheckMultipart(SortedSMS[i]))
            {
                goto cleanup;
            }

            /* Actually process the message */
            handleMessage(SortedSMS[i]);

            /* Delete processed messages */
            for (j = 0; j < SortedSMS[i]->Number; j++)
            {
                SortedSMS[i]->SMS[j].Folder = 0;
                error = GSM_DeleteSMS(_stateMachine, &SortedSMS[i]->SMS[j]);
                switch (error)
                {
                    case ERR_NONE:
                    case ERR_EMPTY:
                        break;
                    default:
                        qWarning("Error deleting SMS (%d): %s", error, GSM_ErrorString(error));
                        return FALSE;
                }
            }

    cleanup:
            free(SortedSMS[i]);
            SortedSMS[i] = NULL;
        }
        free(SortedSMS);
        return TRUE;
    }


    /**
     * Checks whether to process current (possibly) multipart message.
     */
    bool GammuSMSD::SMSD_CheckMultipart(GSM_MultiSMSMessage *MultiSMS)
    {
        bool same_id;
        int current_id;

        /* Does the message have UDH (is multipart)? */
        if (MultiSMS->SMS[0].UDH.Type == UDH_NoUDH || MultiSMS->SMS[0].UDH.AllParts == -1)
        {
            return true;
        }

        /* Grab current id */
        if (MultiSMS->SMS[0].UDH.ID16bit != -1)
        {
             current_id = MultiSMS->SMS[0].UDH.ID16bit;
        }
        else
        {
             current_id = MultiSMS->SMS[0].UDH.ID8bit;
        }

        /* Do we have same id as last incomplete? */
        same_id = (_configIncompleteMessageID != -1 && _configIncompleteMessageID == current_id);

        /* Some logging */
        qDebug("Multipart message 0x%02X, %d parts of %d", current_id, MultiSMS->Number, MultiSMS->SMS[0].UDH.AllParts);

        /* Check if we have all parts */
        if (MultiSMS->SMS[0].UDH.AllParts == MultiSMS->Number)
        {
            goto success;
        }

        /* Have we seen this message recently? */
        if (same_id)
        {
            if (_configIncompleteMessageTime != 0 && difftime(time(NULL),
                    _configIncompleteMessageTime) >= _configMultipartTimeout)
            {
                qDebug("Incomplete multipart message 0x%02X, processing after timeout",
                       _configIncompleteMessageID);

                _configIncompleteMessageID = -1;
            }
            else
            {
                qDebug("Incomplete multipart message 0x%02X, waiting for other parts (waited %.0f seconds)",
                    _configIncompleteMessageID,
                    difftime(time(NULL), _configIncompleteMessageTime));

                return false;
            }
        }
        else
        {
            if (_configIncompleteMessageTime == 0)
            {
                if (MultiSMS->SMS[0].UDH.ID16bit != -1)
                {
                     _configIncompleteMessageID = MultiSMS->SMS[0].UDH.ID16bit;
                }
                else
                {
                     _configIncompleteMessageID = MultiSMS->SMS[0].UDH.ID8bit;
                }
                _configIncompleteMessageTime = time(NULL);
                qDebug("Incomplete multipart message 0x%02X, waiting for other parts",
                    _configIncompleteMessageID);
                return false;
            }
            else
            {
                qDebug("Incomplete multipart message 0x%02X, but waiting for other one",
                    _configIncompleteMessageID);
                return false;
            }
        }

    success:
        /* Clean multipart wait flag */
        if (same_id)
        {
            _configIncompleteMessageTime = 0;
            _configIncompleteMessageID = -1;
        }

        return true;
    }
}
}
