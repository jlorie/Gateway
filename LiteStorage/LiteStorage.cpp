#include "LiteStorage.hpp"

#include "Account.hpp"
#include "Contact.hpp"
#include "Rule.hpp"

#include <QVariant>
#include <QFile>
#include <QHash>

LiteStorage::LiteStorage()
{    
    _db = new QSqlDatabase(QSqlDatabase::addDatabase(QString("QSQLITE")));
}

bool LiteStorage::configure(const StorageConfig &config)
{
    bool result(true);

    QString path(config.value("local_storage_db_path", QString("gateway.db")));
    _db->setDatabaseName(path);

    //opening DB
    if (!QFile::exists(path))
    {
        qDebug("Creating a new sqlite DB: %s", qPrintable(path));

        _db->open();
        if (!createTables())
        {
            qWarning("Error creating DB: %s", qPrintable(_db->lastError().text()));
            result = false;
        }
    }

    if (!_db->isOpen())
        result = _db->open();

    return result;
}

IAccount *LiteStorage::newAccount(const QString &user, const QString &name)
{
    IAccount *account = 0;

    QSqlQuery query;
    QString insertAccount = QString(""
        "INSERT"
        "   INTO account (user, name, balance)"
        "   VALUES ('%1', '%2', '%3')"
    ).arg(user).arg(name).arg(0.0);

    if (executeQuery(insertAccount))
    {
        qlonglong accountId = query.lastInsertId().toLongLong();
        account = new Account(accountId, user, name, _db);
    }
    else
    {
        qWarning("Could not add account %s: %s",qPrintable(user), qPrintable(_db->lastError().text()));
    }

    return account;
}

bool LiteStorage::removeAccount(const qlonglong accountId)
{
    bool result(true);

    QString deleteAccount = QString(""
        "DELETE "
        "   FROM account"
        "   WHERE 'account_id'='%1'"
    ).arg(accountId);

    if (!executeQuery(deleteAccount))
    {
        qWarning("Could not delete account %lld: %s",accountId, qPrintable(_db->lastError().text()));
        result = false;
    }

    return result;
}

AccountList LiteStorage::accounts() const
{
    AccountList accountList;

    QString selectAccounts("SELECT * FROM account");
    executeQuery(selectAccounts);
    while (_query.next())
    {
        qlonglong accountId(_query.value("account_id").toLongLong());
        QString user(_query.value("user").toString());
        QString name(_query.value("name").toString());

        Account *newAccount = new Account(accountId, user, name, _db);

        accountList.append(newAccount);
    }

    return accountList;
}

IRule *LiteStorage::newRule(const QString &from, const QString &to)
{
    Rule *rule = 0;

    QString insertRule = QString(""
        "INSERT"
        "   INTO rule ('from', 'to')"
        "   VALUES ('%1', '%2')"
    ).arg(from).arg(to);

    if (executeQuery(insertRule))
    {
        qlonglong ruleId(_query.lastInsertId().toLongLong());

        rule = new Rule(ruleId, from, to);
    }
    else
    {
        qWarning("Could not add rule %s: %s",qPrintable(rule->from()), qPrintable(_db->lastError().text()));
    }

    return rule;
}

bool LiteStorage::removeRule(const qlonglong ruleId)
{
    bool result(true);

    QString deleteAccount = QString(""
        "DELETE "
        "   FROM rule"
        "   WHERE rule.rule_id='%1'"
    ).arg(ruleId);

    if (!executeQuery(deleteAccount))
    {
        qWarning("Could not delete Rule %lld: %s",ruleId, qPrintable(_db->lastError().text()));
        result = false;
    }

    return result;
}

RuleList LiteStorage::rules() const
{
    RuleList ruleList;

    QString selectAccounts("SELECT * FROM rule");
    executeQuery(selectAccounts);
    while (_query.next())
    {
        qlonglong ruleId(_query.value(0).toLongLong());
        QString from(_query.value(1).toString());
        QString to(_query.value(2).toString());

        ruleList.append(new Rule(ruleId, from, to));
    }

    return ruleList;
}

bool LiteStorage::createTables()
{
    bool result = false;

    if (_db->isOpen())
    {
        QString createAccountTable(""
            "CREATE TABLE account ("
            "   account_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "   user TEXT NOT NULL UNIQUE,"
            "   name TEXT,"
            "   balance REAL"
            ")");

        result = executeQuery(createAccountTable);

        QString createContactTable(""
            "CREATE TABLE contact ("
            "    contact_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    name TEXT,"
            "    number TEXT,"
            "    account_id INTEGER,"
            "    FOREIGN KEY(account_id) REFERENCES account(account_id)"
            ")");

        if (result)
            result = executeQuery(createContactTable);

        QString createRuleTable(""
            "CREATE TABLE rule ("
            "    'rule_id' INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    'from' TEXT,"
            "    'to' TEXT"
            ")");

        if (result)
            result = executeQuery(createRuleTable);

        QString createNumberTable(""
            "CREATE TABLE number ("
            "    'number_id' INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    'number' TEXT,"
            "    account_id INTEGER,"
            "    FOREIGN KEY(account_id) REFERENCES account(account_id)"
            ")");

        if (result)
            result = executeQuery(createNumberTable);
    }

    return result;
}
