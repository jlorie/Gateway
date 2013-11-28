#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "Contact.hpp"

#include <QMap>
#include <QStringList>

namespace Gateway
{
    typedef QStringList NumberList;
    typedef QMap<qlonglong, Contact *> ContactList;

    class Account
    {
    public:
        Account(const qlonglong id, const QString &user, const QString &name)
            :_id(id), _user(user), _name(name), _balance(0.0)
        {}

        qlonglong id() const
        {
            return _id;
        }

        QString user() const
        {
            return _user;
        }

        QString name() const
        {
            return _name;
        }

        void addLocalNumber(const QString &number)
        {
            if (!_numbers.contains(number, Qt::CaseInsensitive))
                _numbers.append(number);
            else
                qWarning("Local number %s for %s already exist", qPrintable(number), qPrintable(_name));
        }

        void removeLocalNumber(const QString &number)
        {
            if (!_numbers.removeAll(number))
                qWarning("Local number %s no not exist for %s ", qPrintable(number), qPrintable(_name));
        }

        NumberList localNumbers() const
        {
            return _numbers;
        }

        double balance() const
        {
            return _balance;
        }

        void setBalance(const double balance)
        {
            _balance = balance;
        }

        void addContact(Contact *contact)
        {
            _contacts.insert(contact->id(), contact);
        }

        void removeContact(const qlonglong contactId)
        {
            if (!_contacts.remove(contactId))
            {
                qWarning("Could not remove number %s from contacts", qPrintable(number));
            }
        }

        ContactList contacts() const
        {
            return _contacts;
        }

        //TODO implementar señal accountChanged

    private:
        qlonglong _id;
        QString _user;
        QString _name;

        NumberList _numbers;
        double _balance;

        ContactList _contacts;
    };

}

#endif // ACCOUNT_HPP
