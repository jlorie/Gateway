#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include "IPhoneNumber.hpp"

#include <QtPlugin>
#include <QList>
#include <QMap>
#include <QString>

typedef QMap<QString, QString> DeviceInfo;
typedef QList<IPhoneNumber *> NumberList;

enum DeviceClass
{
    devGSM,
    devWeb
};

class IDevice
{
    public:
        virtual ulong initialize(const DeviceInfo &info) = 0;
        virtual qlonglong deviceId() = 0;

        virtual NumberList phoneNumbers() const = 0;
        virtual DeviceClass type() const = 0;
};


Q_DECLARE_INTERFACE(IDevice, "Gateway.IDevice")
#endif // IDEVICE_HPP
