#include "LiteStorage.hpp"

#include <QSqlQuery>
#include <QVariant>
#include <QFile>
#include <QHash>

namespace Gateway
{
    LiteStorage::LiteStorage()
    {
        _db = QSqlDatabase::addDatabase("QSQLITE");
    }

    bool LiteStorage::configure(const StorageConfig &config)
    {
        bool result(true);

        QString path(config.value("path", QString("gateway.db")));

        //opening DB

        bool newBD(!QFile::exists(path));

        _db.setDatabaseName(path);
        result = _db.open();

        if (result && newBD) // if the DB do not exist create a new one
        {
            qDebug("Creating a new sqlite DB: %s", qPrintable(path));
            if (!createTables())
            {
                qWarning("Error creating DB");
                result = false;
            }
        }

        return result;
    }

    bool LiteStorage::saveAccounts(const AccountList &accounts)
    {
        bool result(true);

        AccountList newAccounts;
        {
            /**
             * Se guada en un hash la repeticion de cada Account
             * las nuevas Account son las que no se repiten
             */
            QHash<QString, int> hash;

            AccountList oldAccounts = accounts();
            foreach (Account *account, oldAccounts)
            {
                hash[account->user()]++;
            }

            foreach (Account *account, accounts)
            {
                hash[account->user()]++;
            }

            // Se elige las cuentas que no se repiten
            QHash<QString, int>::Iterator it = hash.begin();
            while (it != hash.end())
            {
                if (it.value() == 1)
                {
                    QStringList values = QString(it.key()).split("|");
                    newRules.append(new Rule(values.at(0), values.at(1)));
                }
                it++;
            }
        }

        QSqlQuery query;
        foreach (Account *account, newAccounts)
        {
            QString insertAccount = QString(""
                "INSERT"
                "   INTO accounts (user, name, balance)"
                "   VALUES (%1, %2, %3)"
            ).arg(account->user()).arg(account->name()).arg(account->balance());

            if (!query.exec(insertAccount))
            {
                qWarning("Could not add account %s: %s",qPrintable(account->user()), qPrintable(_db.lastError().text()));
                result = false;
            }
        }

        return result;
    }

    AccountList LiteStorage::accounts() const
    {
        AccountList accountList;

        QSqlQuery query;
        QString selectAccounts("SELECT * FROM accounts");
        query.exec(selectAccounts);
        while (query.next())
        {            
            qlonglong accountId(query.value("account_id").toLongLong());
            QString user(query.value("user").toString());
            QString name(query.value("name").toString());
            double balance(query.value("balance").toDouble());

            Account *newAccount = new Account(user, name);
            newAccount->setBalance(balance);

            // fetching contacts
            {
                QString selectContacts = QString(""
                    "SELECT * FROM accounts WHERE contact_account=%1").arg(accountId);

                QSqlQuery contactQuery;
                contactQuery.exec(selectContacts);

                while (query.next())
                {
                    qlonglong contactId(contactQuery.value("contactId").toLongLong());
                    Q_UNUSED(contactId);

                    QString name(contactQuery.value(1).toString());
                    QString number(contactQuery.value(2).toString());

                    newAccount->addContact(new Contact(name, number));
                }
                // append contacts to newAccount
            }

            accountList.append(newAccount);
        }

        return accountList;
    }

    bool LiteStorage::saveRules(const RuleList &ruleList)
    {
        bool result(true);

        //checking new rules
        RuleList newRules;
        {
            /**
             * Se guada en un hash la repeticion de cada Rule
             * las nuevas Rule son las que no se repiten
             */
            QHash<QString, int> hash;

            RuleList oldRules = rules();
            foreach (Rule *rule, oldRules)
            {
                QString key = rule->from() + QString("|") +rule->to();
                hash[key]++;
            }

            foreach (Rule *rule, ruleList)
            {
                QString key = rule->from() + QString("|") +rule->to();
                hash[key]++;
            }

            // Se elige las reglas que no se repiten
            QHash<QString, int>::Iterator it = hash.begin();
            while (it != hash.end())
            {
                if (it.value() == 1)
                {
                    QStringList values = QString(it.key()).split("|");
                    newRules.append(new Rule(values.at(0), values.at(1)));
                }
                it++;
            }
        }


        QSqlQuery query;
        foreach (Rule *rule, newRules)
        {
            QString insertRule = QString(""
                "INSERT"
                "   INTO accounts (from, to)"
                "   VALUES (%1, %2)"
            ).arg(rule->from()).arg(rule->to());

            if (!query.exec(insertRule))
            {
                qWarning("Could not add rule %s: %s",qPrintable(rule->from()), qPrintable(_db.lastError().text()));

                result = false;
            }
        }

        return result;
    }

    RuleList LiteStorage::rules() const
    {
        RuleList ruleList;

        QSqlQuery query;
        QString selectAccounts("SELECT * FROM rules");
        query.exec(selectAccounts);
        while (query.next())
        {
            qlonglong ruleId(query.value(0).toLongLong());
            Q_UNUSED(ruleId);

            QString from(query.value(1).toString());
            QString to(query.value(2).toString());

            ruleList.append(new Rule(from, to));
        }

        return ruleList;
    }

    bool LiteStorage::createTables()
    {
        bool result = false;
        if (_db.isOpen())
        {
            QSqlQuery query;

            QString createAccountTable(""
                "CREATE TABLE account ("
                "   account_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "   user TEXT NOT NULL UNIQUE,"
                "   name TEXT,"
                "   balance REAL"
                ")");

            result = query.exec(createAccountTable);

            QString createContactTable(""
                "CREATE TABLE contact ("
                "    contact_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    name TEXT,"
                "    number TEXT,"                
                "    account_id INTEGER,"
                "    FOREIGN KEY(account_id) REFERENCES account(account_id)"
                ")");

            if (result)
                result = query.exec(createContactTable);

            QString createRuleTable(""
                "CREATE TABLE rule ("
                "    'rule_id' INTEGER PRIMARY KEY AUTOINCREMENT,"
                "    'from' TEXT,"
                "    'to' TEXT"
                ")");
	    
            if (result)
                result = query.exec(createRuleTable);
	    
	    QString createNumberTable(""
	    "CREATE TABLE number ("
	    "	number_id INTEGER PRIMARY KEY AUTOINCREMENT,"
	    "	number TEXT NOT NULL UNIQUE,"
	    "	account_id INTEGER,"
	    "	FOREIGN KEY(account_id) REFERENCES account(account_id)"
	    ")");
	    
	    if (result)
	      result = query.exec(createRuleTable);


        }

        return result;
    }
}
