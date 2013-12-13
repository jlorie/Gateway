#ifndef LITESTORAGE_HPP
#define LITESTORAGE_HPP

#include <include/IStorage.hpp>

#include <QObject>
#include <QtPlugin>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class LiteStorage : public QObject, public IStorage
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Gateway.IStorage")
    Q_INTERFACES(IStorage)

    public:
        LiteStorage();

        bool configure(const StorageConfig &config);

        IRule *ruleFor(const IRule *rule) const;

    private:
        bool openDB(const QString &path);

        inline bool executeQuery(const QString &query) const
        {
            _query = _db->exec(query);
            return (_query.lastError().type() == QSqlError::NoError);
        }

    private:
        QSqlDatabase *_db;
        mutable QSqlQuery _query;

};
#endif // LITESTORAGE_HPP
