#ifndef RULE_HPP
#define RULE_HPP

#include "IRule.hpp"
#include <QString>

using namespace Gateway;

class Rule : public IRule
{
public:
    Rule(const qlonglong id, const QString &from, const QString &to)
        :_id(id), _from(from), _to(to)
    {}

    qlonglong id() const
    {
        return _id;
    }

    QString from() const
    {
        return _from;
    }

    void setFrom(const QString &from)
    {
        _from = from;
    }

    QString to() const
    {
        return _to;
    }

    void setTo(const QString &to)
    {
        _to = to;
    }

private:
    qlonglong _id;
    QString _from;
    QString _to;
};

#endif // RULE_HPP
