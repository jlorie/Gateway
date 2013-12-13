#ifndef ISTORAGE_HPP
#define ISTORAGE_HPP

#include <QtPlugin>
#include <QString>
#include <QMap>

#include "IRule.hpp"

typedef QMap<QString, QString> StorageConfig;
typedef QList<IRule *> RuleList;

class IStorage
{

public:
    virtual bool configure(const StorageConfig &config) = 0;

    virtual IRule *ruleFor(const IRule *rule) const = 0;
};

Q_DECLARE_INTERFACE(IStorage, "Gateway.IStorage")
#endif // ISTORAGE_HPP
