#ifndef IRULE_HPP
#define IRULE_HPP

#include <QString>

class IRule
{
public:
    virtual QString from() const = 0;

    virtual QString to() const = 0;
};

#endif // RULE_HPP
