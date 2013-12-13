#ifndef RULE_HPP
#define RULE_HPP

#include <include/IRule.hpp>
#include <QString>

class Rule : public IRule
{
public:
    Rule(const qlonglong id, const QString &from, const QString &to, const qlonglong redirectRuleId = 0)
        :_id(id), _from(from), _to(to), _redirectRuleId(redirectRuleId)
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

    qlonglong redirectRuleId() const
    {
        return _redirectRuleId;
    }


private:
    qlonglong _id;    
    QString _from;
    QString _to;
    qlonglong _redirectRuleId;
};

#endif // RULE_HPP
