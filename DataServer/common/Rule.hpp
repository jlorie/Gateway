#ifndef RULE_HPP
#define RULE_HPP

#include <include/IRule.hpp>

namespace Gateway
{
    class Rule: public IRule
    {
        public:
            Rule(const QString &from, const QString &to)
                :_from(from), _to(to)
            {}

            QString from() const
            {
                return _from;
            }

            QString to() const
            {
                return _to;
            }

        private:
            QString _from;
            QString _to;

    };
}
#endif // RULE_HPP
