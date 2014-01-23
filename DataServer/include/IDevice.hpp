#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include "IPhoneNumber.hpp"

#include <QList>
#include <QObject>

typedef QList<IPhoneNumber *> NumberList;

class IDevice : public QObject
{
    Q_OBJECT
    public:
        virtual ~IDevice() {}

        virtual qlonglong deviceId() = 0;
        virtual NumberList phoneNumbers() const = 0;
};

#endif // IDEVICE_HPP
