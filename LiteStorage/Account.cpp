#include <Account.hpp>

qlonglong Account::id() const
{
    return _id;
}

QString Account::user() const
{
    return _user;
}

QString Account::name() const
{
    return _name;
}

void Account::addLocalNumber(const QString &number)
{
    QString insertNumber = QString(""
         "INSERT"
         "    INTO number ('number', 'account_id')"
         "    VALUES ('%1', %2)"
    ).arg(number).arg(_id);

    QSqlQuery query(_db->exec(insertNumber));
    if (query.lastError().type() != QSqlError::NoError)
    {
        qWarning("Could not add number %s: %s", qPrintable(number),
                 qPrintable(query.lastError().text()));
    }
}

void Account::removeLocalNumber(const QString &number)
{
    QString deleteNumber = QString(""
       "DELETE "
       "   FROM number"
       "   WHERE 'number'='%1'"
    ).arg(number);

    QSqlQuery query(_db->exec(deleteNumber));
    if (query.lastError().type() != QSqlError::NoError)
    {
        qWarning("Could not remove number %s: %s", qPrintable(number),
                 qPrintable(query.lastError().text()));
    }
}

NumberList Account::localNumbers() const
{
    NumberList numbers;

    QString selectNumbers = QString(""
       "SELECT *"
       "   FROM number"
    );

    QSqlQuery query(_db->exec(selectNumbers));
    while (query.next())
    {
        numbers.append(query.value("number").toString());
    }

    return numbers;
}

double Account::balance() const
{
    double balance = 0;

    QString selectBalance = QString(""
       "SELECT	balance"
       "	FROM account "
       "    WHERE account_id = %1"
    ).arg(_id);

    QSqlQuery query(_db->exec(selectBalance));
    if (query.next())
        balance = query.value("number").toDouble();

    return balance;
}

void Account::setBalance(const double balance)
{
    QString updateBalance = QString(""
       "UPDATE account"
       "	SET balance = %1"
       "    WHERE account_id = %2"
    ).arg(balance, _id);

    QSqlQuery query(_db->exec(updateBalance));
    if (query.lastError().type() != QSqlError::NoError)
    {
        qWarning("Could not update balance to %f", balance);
    }
}

IContact *Account::newContact(const QString &name, const QString &number)
{
    Contact *contact = 0;
    QSqlQuery query;
    QString insertAccount = QString(""
        "INSERT"
        "    INTO contact ('name', 'number', 'account_id')"
        "    VALUES ('%1', '%2', %3)"
    ).arg(name).arg(number).arg(_id);

    if (query.exec(insertAccount))
    {
        qlonglong contactId = query.lastInsertId().toLongLong();
        contact = new Contact(contactId, name, number);
    }
    else
    {
        qWarning("Could not add contact %s: %s",qPrintable(name), qPrintable(_db->lastError().text()));
    }

    return contact;
}

void Account::removeContact(const qlonglong contactId)
{
    QSqlQuery query;
    QString deleteContact = QString(""
        "DELETE "
        "    FROM contact"
        "    WHERE contact_id=%1"
    ).arg(contactId);

    if (!query.exec(deleteContact))
    {
        qWarning("Could not remove number %lld from contacts", contactId);
    }
}

ContactList Account::contacts() const
{
    ContactList result;

    QString selectContact = QString(""
        "SELECT *"
        "    FROM contact"
    );

    QSqlQuery query(_db->exec(selectContact));
    while (query.next())
    {
        qlonglong contactId(query.value("contact_id").toLongLong());
        QString name(query.value("name").toString());
        QString number(query.value("number").toString());

        result.append(new Contact(contactId, name, number));
    }

    return result;
}
