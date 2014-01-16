#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>

class PhoneNumber : public IPhoneNumber
{
    public:
        PhoneNumber(const QString &number);
        ~PhoneNumber();

        QString number() const;
        void sendMessage(const IMessage *message);
        MessageList unreadMessages() const;
};



#endif // PHONENUMBER_HPP
