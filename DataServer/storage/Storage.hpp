#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <include/IStorage.hpp>

#include <QObject>

namespace Gateway
{
    class Storage : public QObject
    {
        Q_OBJECT
    public:
        Storage();

        virtual IAccount *newAccount(const QString &user, const QString &name);
        virtual bool removeAccount(const qlonglong accountId);
        virtual AccountList accounts() const;

        virtual IRule *newRule(const QString &from, const QString &to);
        virtual bool removeRule(const qlonglong ruleId);
        virtual RuleList rules() const;

    protected:
        IStorage *_storage;

    };
}
#endif // STORAGE_HPP
