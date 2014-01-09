#ifndef UTILS_HPP
#define UTILS_HPP

#include <QString>

namespace Util
{
    static QString trim(const QString &string)
    {
        QString result = string.trimmed();
        int pos(0);
        while (result[pos] == '"')
            pos++;

        result = result.mid(pos);

        pos = result.length() - 1;
        while (result[pos] == '"')
            pos--;

        result.truncate(pos + 1);

        return result;
    }
}

#endif // UTILS_HPP
