#ifndef IMESSAGE_HPP
#define IMESSAGE_HPP

#include <QString>

class IMessage
{
    public:
        virtual qlonglong id() const = 0;
        virtual QString from() const = 0;
        virtual QString to() const = 0;

        virtual QString body() const = 0;
};


#endif // IMESSAGE_HPP
