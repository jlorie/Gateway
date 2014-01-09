#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include "IPhoneNumber.hpp"

#include <QList>
#include <QMap>
#include <QString>
#include <QObject>

typedef QList<IPhoneNumber *> NumberList;
typedef QMap<QString, QString> DeviceInfo;

enum DeviceClass
{
    devGSM,
    devWeb
};

class IDevice : public QObject
{
    Q_OBJECT
    public:
        virtual ~IDevice() {}

        virtual qlonglong deviceId() = 0;

        virtual NumberList phoneNumbers() const = 0;
        virtual DeviceClass type() const = 0;

    signals:
        void printed();
};

#endif // IDEVICE_HPP
