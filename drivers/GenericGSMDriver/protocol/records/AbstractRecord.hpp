#ifndef ABSTRACTRECORD_HPP
#define ABSTRACTRECORD_HPP

#include <QMap>

enum RecordType
{
    recInconmingMessage
};

class AbstractRecord
{
public:
    AbstractRecord()
        :_valid(true)
    {}

    virtual void fromRawString(const QString &data)
    {
        Q_UNUSED(data);
    }

    virtual bool isValid()
    {
        return _valid;
    }

    virtual RecordType type() const = 0;

protected:
    QMap<QString, QString> _fields;
    bool _valid;
};
#endif // ABSTRACTRECORD_HPP
