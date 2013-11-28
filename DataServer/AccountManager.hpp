#ifndef ACCOUNTMANAGER_HPP
#define ACCOUNTMANAGER_HPP

#include <storage/LocalStorage.hpp>

#include <QObject>

namespace Gateway
{
    class AccountManager : public QObject
    {
        Q_OBJECT
    public:
        static void initialize();
        static AccountManager *instance();
        static void destroyInstance();

        void addAccount(const QString &user, const QString &name);
        void removeAccount(const qlonglong accountId);
        AccountList accounts() const;

    private:
        AccountManager();

    private:
        static AccountManager *_instance;

        LocalStorage *_localStorage;
    };
}
#endif // ACCOUNTMANAGER_HPP
