#include "DeviceManager.hpp"
#include "DriverManager.hpp"
#include "RemoteStorage.hpp"

#include <SystemConfig.hpp>
#include <include/CommonErrors.hpp>
#include <include/IDevice.hpp>

#include <QDebug>
#include <QThread>

#include <QDebug>

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
                qDebug("Device with IMEI %s has been initialized ...", qPrintable(device->deviceId()));

                _devices.append(device);
                if (info.contains("device_phonenumber"))
                {
                    QString number(info.value("device_phonenumber"));
                    _numbers.append(new NumberInfo(number, device));

                    qDebug("New phone number %s has been registered", qPrintable(number));

                    //re-emitting signal
                    connect(device, SIGNAL(newMessageReceived(const IMessage*)),
                            this, SIGNAL(newMessageReceived(const IMessage*)));

                    connect(device, SIGNAL(messageStatusChanged(qlonglong,MessageStatus)),
                            RemoteStorage::instance(), SLOT(notifyMessageStatus(qlonglong,MessageStatus)));

                    QThread *thread = new QThread;
                    device->moveToThread(thread);
                    thread->start();
                }
                else
                {
                    qWarning("Error phone numbers were not found for device %s", qPrintable(device->deviceId()));
                    result = Error::errNumberNotFound;
                }

            }
            else
            {
                qWarning("Cannot create instance for device: %s", qPrintable(info.value("device_id")));
                result = Error::errDeviceNotInitialized;
            }
        }

        return result;
    }

    ulong DeviceManager::deleteDevice(const QString &deviceId)
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

    IDevice *DeviceManager::deviceForId(const QString &deviceId) const
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

    NumberInfo *DeviceManager::phoneForNumber(const QString &number) const
    {
        NumberInfo *result = 0;
        foreach (NumberInfo *info, _numbers)
        {
            if (info->number() == number)
            {
                result = info;
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
