#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <include/IDevice.hpp>
#include <include/IMessage.hpp>
#include <include/DriverInterface.hpp>

#include <QObject>
#include <QLibrary>

namespace Gateway
{
    typedef QList<DriverInterface *> LibraryList;
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

    signals:

    public slots:
        void redirectSMS(const IMessage *message);

    protected slots:
        void messageSentNotification(const IMessage *message);

    private:
        DeviceManager(QObject *parent = 0);
        ~DeviceManager();

        void loadDrivers(const QString &drivers_path);
        void browseSerialDevices();
    private:
        static DeviceManager *_instance;

        DeviceList _devices;
        NumberList _numbers;
        LibraryList _driverLibraries;
    };
}
#endif // DEVICEMANAGER_HPP
