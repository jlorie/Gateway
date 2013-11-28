#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <IAccount.hpp>
#include "Contact.hpp"

#include <QMap>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

using namespace Gateway;

class Account : public IAccount
{
public:
    Account(const qlonglong id, const QString &user, const QString &name, QSqlDatabase *db)
        :_id(id), _user(user), _name(name), _balance(0.0), _db(db)
    {}

    qlonglong id() const;
    QString user() const;
    QString name() const;

    void addLocalNumber(const QString &number);
    void removeLocalNumber(const QString &number);
    NumberList localNumbers() const;

    double balance() const;
    void setBalance(const double balance);

    IContact *newContact(const QString &name, const QString &number);
    void removeContact(const qlonglong contactId);
    ContactList contacts() const;

    //TODO implementar señal accountChanged

private:
    qlonglong _id;
    QString _user;
    QString _name;

    double _balance;

    NumberList _numbers;
    ContactList _contacts;

    QSqlDatabase *_db;
};

#endif // ACCOUNT_HPP
