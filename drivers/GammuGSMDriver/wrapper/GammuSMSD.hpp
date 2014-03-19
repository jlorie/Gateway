#ifndef GAMMUSMSD_HPP
#define GAMMUSMSD_HPP

#include <gammu.h>
#include <QString>
#include <QList>
#include <QPair>

#include <QThread>
#include <QMutex>

#include <include/IMessage.hpp>

namespace Gateway
{
namespace Driver
{
    typedef QPair<qlonglong, GSM_SMSMessage> SmsData;
    typedef QList<SmsData> SmsList;

    class GammuSMSD: public QThread
    {
    Q_OBJECT
    public:
        GammuSMSD(const QString &serialPort);
        ~GammuSMSD();

        bool init();
        void run();
        void stop();

        void injectSms(const QString &to, const QString &body, qlonglong id);

        QString imsi() const;

    signals:
        void messageReceived(const QString &from, const QString body);
        void messageStatusChanged(const qlonglong &id, const MessageStatus status);
        void connectionTerminated();

    protected:
        void handleMessage(GSM_MultiSMSMessage *sms);
        bool sendQueuedMessages();

    private slots:
        void onMessageStatusChanged(const qlonglong &id, const MessageStatus status);

    private:
        void mainLoop();
        void SMSD_Terminate(GSM_Error error);
        bool SMSD_CheckMultipart(GSM_MultiSMSMessage *MultiSMS);
        bool SMSD_ReadDeleteSMS();
        bool SMSD_CheckSMSStatus();

    private:
        GSM_Config *cfg;
        GSM_StateMachine * _stateMachine;
        QString _serialPort;
        GSM_SMSC PhoneSMSC;

        SmsList _smsToSend;
        QMutex _mutex;

        bool _configShutdown;
        char _configStatusIMSI[15];
        int _configReceiveFrequency;
        int _configIncompleteMessageID;
        int _configIncompleteMessageTime;
        int _configMultipartTimeout;
        int _configResetFrequency;
        int _configHardResetFrequency;
        int _configCommTimeout;
        int _configLoopSleep;
        uint _configSendTimeout;
        int _configCurrDeliveryReport;
        int _configRelativeValidity;
        GSM_SMSC _SMSCCache;
    };
}
}
#endif // GAMMUSMSD_HPP
