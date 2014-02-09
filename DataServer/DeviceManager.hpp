#ifndef DEVICEMANAGER_HPP
#define DEVICEMANAGER_HPP

#include <include/IDevice.hpp>
#include <include/IMessage.hpp>
#include <include/DriverInterface.hpp>
#include <include/DataStructures/NumberInfo.hpp>

#include <QObject>
#include <QLibrary>

namespace Gateway
{
    typedef QList<IDevice *> DeviceList;
    typedef QList<NumberInfo *> NumberList;

    class DeviceManager : public QObject
    {
        Q_OBJECT
    public:
        static void initialize();
        static DeviceManager * instance();
        static void destroyInstance();

        bool createDevice(const DeviceInfo &info);
        bool deleteDevice(const QString &deviceId);

        IDevice *deviceForId(const QString &deviceId) const;
        NumberInfo *phoneForNumber(const QString &number) const;

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
