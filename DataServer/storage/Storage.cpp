#include "LocalStorage.hpp"

#include <QFile>

namespace Gateway
{
    Storage::Storage()
        :_storage(0)
    {}

    IAccount *Storage::newAccount(const QString &user, const QString &name)
    {
        return _storage->newAccount(user, name);
    }

    bool Storage::removeAccount(const qlonglong accountId)
    {
        return _storage->removeAccount(accountId);
    }

    AccountList Storage::accounts() const
    {
        return _storage->accounts();
    }

    IRule *Storage::newRule(const QString &from, const QString &to)
    {
        return _storage->newRule(from, to);
    }

    bool Storage::removeRule(const qlonglong ruleId)
    {
        return _storage->removeRule(ruleId);
    }

    RuleList Storage::rules() const
    {
        return _storage->rules();
    }

}
