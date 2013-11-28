#ifndef CONTACT_HPP
#define CONTACT_HPP

#include "IContact.hpp"

#include <QString>
#include <QHash>

using namespace Gateway;

class Contact : public IContact
{
public:
    Contact(const qlonglong id, const QString &name, const QString &number)
        :_id(id), _name(name), _number(number)
    {}

    qlonglong id() const
    {
        return _id;
    }

    QString name() const
    {
        return _name;
    }

    void setName(const QString &name)
    {
        _name = name;
    }

    QString number() const
    {
        return _number;
    }

    void setNumber(const QString &number)
    {
        _number = number;
    }

private:
    qlonglong _id;
    QString _name;
    QString _number;
};

#endif // CONTACT_HPP
