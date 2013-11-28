#ifndef IACCOUNT_HPP
#define IACCOUNT_HPP

#include "IContact.hpp"

#include <QMap>
#include <QStringList>

namespace Gateway
{
    typedef QStringList NumberList;
    typedef QList<IContact *> ContactList;

    class IAccount
    {
    public:
        virtual qlonglong id() const = 0;

        virtual QString user() const = 0;

        virtual QString name() const = 0;

        virtual void addLocalNumber(const QString &number) = 0;

        virtual void removeLocalNumber(const QString &number) = 0;

        virtual NumberList localNumbers() const = 0;

        virtual double balance() const = 0;

        virtual void setBalance(const double balance) = 0;

        virtual IContact *newContact(const QString &name, const QString &number) = 0;

        virtual void removeContact(const qlonglong contactId) = 0;

        virtual ContactList contacts() const = 0;
    };

}

#endif // ACCOUNT_HPP
