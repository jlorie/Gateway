#ifndef LITESTORAGE_HPP
#define LITESTORAGE_HPP

#include <include/IStorage.hpp>

#include <QSqlDatabase>
#include <QSqlError>

namespace Gateway
{
    class LiteStorage : public IStorage
    {
    public:
        LiteStorage();

        bool configure(const StorageConfig &config);

         bool addAccount(const Account *account);
         bool removeAccount(const Account *account);
         AccountList accounts() const;

         bool addRule(const Rule* rule);
         bool removeRule(const Rule* rule);
         RuleList rules() const;

    private:
        bool openDB(const QString &path);
        bool createTables();

    private:
        QSqlDatabase _db;

    };
}
#endif // LITESTORAGE_HPP
