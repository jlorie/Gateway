#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <include/IDevice.hpp>
#include <include/IMessage.hpp>
#include <include/DriverInterface.hpp>

#include <QObject>
#include <QLibrary>

namespace Gateway
{
    typedef QList<IDevice *> DeviceList;

    class DeviceManager : public QObject
    {
        Q_OBJECT
    public:
        static void initialize();
        static DeviceManager * instance();
        static void destroyInstance();

        ulong createDevice(const DeviceInfo &info);
        ulong deleteDevice(const qlonglong deviceId);

        IDevice *deviceForId(qlonglong deviceId) const;
        IPhoneNumber *phoneForNumber(const QString &number) const;

    signals:
         void newMessageReceived(const IMessage *message);

    private:
        DeviceManager();
        ~DeviceManager();

    private:
        static DeviceManager *_instance;

        DeviceList _devices;
        NumberList _numbers;
    };
}
#endif // DEVICEMANAGER_HPP
