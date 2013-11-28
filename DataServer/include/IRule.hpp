#ifndef IRULE_HPP
#define IRULE_HPP

#include <QString>

namespace Gateway
{
    class IRule
    {
    public:
        virtual qlonglong id() const = 0;

        virtual QString from() const = 0;

        virtual void setFrom(const QString &from) = 0;

        virtual QString to() const = 0;

        virtual void setTo(const QString &to) = 0;
    };
}

#endif // RULE_HPP
