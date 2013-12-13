#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>

class PhoneNumber : public IPhoneNumber
{
    public:
        explicit PhoneNumber();

        QString number() const;
        void sendMessage(const IMessage *message);
        MessageList unreadMessages() const;

        double balance() const;
};

#endif // PHONENUMBER_HPP
