#ifndef PHONENUMBER_HPP
#define PHONENUMBER_HPP

#include <include/IPhoneNumber.hpp>
#include <physical/SerialPhysicalLayer.hpp>

class PhoneNumber : public IPhoneNumber
{
    public:
        explicit PhoneNumber(SerialPhysicalLayer *physical);

        QString number() const;
        void sendMessage(const IMessage *message);
        MessageList unreadMessages() const;
    private:
        SerialPhysicalLayer *_physical;

};

#endif // PHONENUMBER_HPP
