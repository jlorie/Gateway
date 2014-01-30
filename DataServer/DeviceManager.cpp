#include "DeviceManager.hpp"
#include "DriverManager.hpp"

#include <QDebug>
#include <QThread>

#include <QDebug>

#include <SystemConfig.hpp>
#include <include/CommonErrors.hpp>
#include <include/IDevice.hpp>

namespace Gateway
{
    DeviceManager * DeviceManager::_instance = 0;

    void DeviceManager::initialize()
    {
        if (!_instance)
        {
            qDebug(">> Initializing DeviceManager ...");
            _instance = new DeviceManager;
            qDebug(">> DeviceManager initialized !!!");
        }
    }

    DeviceManager *DeviceManager::instance()
    {
        return _instance;
    }

    void DeviceManager::destroyInstance()
    {
        if (_instance)
        {
            delete _instance;
        }

        _instance = 0;
    }

    ulong DeviceManager::createDevice(const DeviceInfo &info)
    {
        ulong result = Error::OK;

        IDevice *device = 0;
        {
            QString driverName = info.value(QString("driver_name"), QString("GenericGSMDriver"));

            DriverInterface *driver = DriverManager::instance()->driverFor(driverName);

            if (!driver)
            {
                qWarning("Could not find driver %s", qPrintable(driverName));
                return Error::errDriverNotFound;
            }

            device = driver->newDevice(info);

            if (device)
            {
                qDebug("Device %s initialized ...", qPrintable(info.value(QString("device_id"), QString("Unknown"))));

                _devices.append(device);
                foreach (IPhoneNumber *phoneNumber, device->phoneNumbers())
                {
                    qDebug("New phone number (%s) has been registered", qPrintable(phoneNumber->number()));
                    _numbers.append(phoneNumber);

                    //re-emitting signal
                    connect(phoneNumber, SIGNAL(newMessageReceived(const IMessage*)),
                            this, SIGNAL(newMessageReceived(const IMessage*)));
                }

                QThread *thread = new QThread;
                device->moveToThread(thread);
                thread->start();
            }
            else
            {
                qWarning("Cannot create instance for device: %s", qPrintable(info.value("device_id")));
                result = Error::errDeviceNotInitialized;
            }
        }

        return result;
    }

    ulong DeviceManager::deleteDevice(const qlonglong deviceId)
    {
        ulong result = Error::OK;

        IDevice *deviceToDelete = deviceForId(deviceId);

        if (deviceToDelete)
        {            
//            deviceToDelete->deleteLater();
        }
        else
        {
            result = Error::errDeviceNotFound;
        }

        return result;
    }

    IDevice *DeviceManager::deviceForId(qlonglong deviceId) const
    {
        IDevice *result = 0;

        foreach (IDevice *device, _devices)
        {
            if (device->deviceId() == deviceId)
            {
                result = device;
                break;
            }
        }

        return result;
    }

    IPhoneNumber *DeviceManager::phoneForNumber(const QString &number) const
    {
        IPhoneNumber *result = 0;
        foreach (IPhoneNumber *phoneNumber, _numbers)
        {
            if (phoneNumber->number() == number)
            {
                result = phoneNumber;
                break;
            }
        }

        return result;
    }

    DeviceManager::DeviceManager()
    {
        SystemConfig *config = SystemConfig::instance();

        //Creating Serial Devices
        foreach (DeviceInfo devInfo, config->devicesInfo())
        {
            if (devInfo.isEnabled())
            {
                qDebug(">> Creating device %s", qPrintable(devInfo.value("device_id")));
                createDevice(devInfo);
            }
        }
    }

    DeviceManager::~DeviceManager()
    {

    }
}
