#ifndef ISTORAGE_HPP
#define ISTORAGE_HPP

#include "IAccount.hpp"
#include "IRule.hpp"

#include <QList>
#include <QMap>

namespace Gateway
{
    typedef QMap<QString, QString> StorageConfig;
    typedef QList<IAccount *> AccountList;
    typedef QList<IRule *> RuleList;

    class IStorage
    {
    public:
        virtual bool configure(const StorageConfig &config) = 0;

        virtual IAccount *newAccount(const QString &user, const QString &name) = 0;
        virtual bool removeAccount(const qlonglong accountId) = 0;
        virtual AccountList accounts() const = 0;

        virtual IRule *newRule(const QString &from, const QString &to) = 0;
        virtual bool removeRule(const qlonglong ruleId) = 0;
        virtual RuleList rules() const = 0;
    };
}

#endif // ISTORAGE_HPP
