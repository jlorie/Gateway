#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include "devices/AbstractGSMDevice.hpp"
#include "devices/DeviceInfo.hpp"
#include <webapi/TelAPISMS.hpp>

#include <QObject>

namespace Gateway
{
    class DeviceManager : public QObject
    {
        Q_OBJECT
    public:
        static void initialize();
        static DeviceManager * instance();
        static void destroyInstance();

        ulong createDevice(const DeviceInfo &info);
        ulong deleteDevice(const qlonglong deviceID);

        AbstractGSMDevice *deviceForId(qlonglong deviceID) const;

    signals:

    public slots:
        void redirectSMS(const SMS &sms);        

    private:
        DeviceManager(QObject *parent = 0);
        void browseSerialDevices();
        QString ruleFor(const QString &from);

    private:
        static DeviceManager *_instance;
        QList<AbstractGSMDevice *> _devices;
        TelAPI *_webapi;

    };
}
#endif // DEVICEMANAGER_HPP
