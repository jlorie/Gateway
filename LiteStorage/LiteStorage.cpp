#include "LiteStorage.hpp"

#include "Rule.hpp"

#include <QVariant>
#include <QFile>

LiteStorage::LiteStorage()
{
    _db = new QSqlDatabase(QSqlDatabase::addDatabase(QString("QSQLITE")));
}

bool LiteStorage::configure(const StorageConfig &config)
{
    QString path(config.value("local_storage_db_path", QString("gateway.db")));
    _db->setDatabaseName(path);

    //opening DB
    if (!_db->open())
    {
        qWarning("Error opening database: %s", qPrintable(_db->lastError().text()));

        return false;
    }

    return true;
}

IRule *LiteStorage::ruleFor(const IRule *rule) const
{
    Rule *resultRule(0);

    QString selectRedirectRule = QString(
                "SELECT redirect_rule_id FROM rule "
                "WHERE rule.'from'= '%1' and rule.'to'= '%2'"
            ).arg(rule->from(), rule->to());

    executeQuery(selectRedirectRule);

    if (_query.first())
    {
        qlonglong redirectRuleId(_query.value(QString("redirect_rule_id")).toLongLong());
        QString selectRule = QString(
                                "SELECT * FROM rule "
                                "WHERE rule.'rule_id' = %1"
                              ).arg(redirectRuleId);

        executeQuery(selectRule);
        if (_query.first())
        {
            const qlonglong ruleId(_query.value("rule_id").toLongLong());
            const QString from(_query.value("from").toString());
            const QString to(_query.value("to").toString());

            resultRule = new Rule(ruleId, from, to);
        }

    }

    return resultRule;
}
