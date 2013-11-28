#ifndef LITESTORAGE_HPP
#define LITESTORAGE_HPP

#include <IStorage.hpp>
#include "litestorage_global.hpp"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

using namespace Gateway;

class LiteStorage : public IStorage
{
public:
    LiteStorage();

    bool configure(const StorageConfig &config);

    IAccount *newAccount(const QString &user, const QString &name);
    bool removeAccount(const qlonglong accountId);
    AccountList accounts() const;

    IRule *newRule(const QString &from, const QString &to);
    bool removeRule(const qlonglong ruleId);
    RuleList rules() const;

private:
    bool openDB(const QString &path);
    bool createTables();

    mutable QSqlQuery _query;

    inline bool executeQuery(const QString &query) const
    {
        _query = _db->exec(query);
        return (_query.lastError().type() == QSqlError::NoError);
    }

private:
    QSqlDatabase *_db;    

};

#endif // LITESTORAGE_HPP
