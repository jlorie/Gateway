#ifndef ICONTACT_HPP
#define ICONTACT_HPP

#include <QString>
#include <QHash>

namespace Gateway
{
    class IContact
    {
    public:
        virtual qlonglong id() const = 0;

        virtual QString name() const = 0;

        virtual void setName(const QString &name) = 0;

        virtual QString number() const = 0;

        virtual void setNumber(const QString &number) = 0;
    };
}

#endif // CONTACT_HPP
