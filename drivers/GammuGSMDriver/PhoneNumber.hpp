#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>
#include <gammu.h>

#include <QTimer>

namespace Gateway
{
namespace Driver
{
    class PhoneNumber : public IPhoneNumber
    {
        Q_OBJECT
    public:
        PhoneNumber(const QString &number, GSM_StateMachine *stateMachine);
        ~PhoneNumber();

        QString number() const;
        void sendMessage(const IMessage *message);

    protected slots:
        void checkForNewMessage();
        void discardMessages();

//    private:
//        void send_sms_callback(GSM_StateMachine *sm, int status, int MessageReference, void *user_data);

    private:
        QString _number;
        GSM_StateMachine *_stateMachine;

        QTimer _timer;
        bool _shutdown;
    };
}
}



#endif // PHONENUMBER_HPP
