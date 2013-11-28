#include "AccountManager.hpp"

namespace Gateway
{
    AccountManager * AccountManager::_instance = 0;

    AccountManager *AccountManager::instance()
    {
        return _instance;
    }

    void AccountManager::initialize()
    {
        if (!_instance)
            _instance = new AccountManager;
    }

    void AccountManager::destroyInstance()
    {
        delete _instance;
    }

    void AccountManager::addAccount(const QString &user, const QString &name)
    {
        _localStorage->newAccount(user, name);
    }

    void AccountManager::removeAccount(const qlonglong accountId)
    {
        _localStorage->removeAccount(accountId);
    }

    AccountList AccountManager::accounts() const
    {
        return _localStorage->accounts();
    }

    AccountManager::AccountManager()
    {
        _localStorage = LocalStorage::instance();
    }
}
