#include "DeviceManager.hpp"
#include "DriverManager.hpp"
#include "RemoteStorage.hpp"

#include <SystemConfig.hpp>
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

    bool DeviceManager::createDevice(const DeviceInfo &info)
    {
        bool result(true);

        IDevice *device = 0;
        {
            QString driverName = info.value(QString("driver_name"), QString("GenericGSMDriver"));

            DriverInterface *driver = DriverManager::instance()->driverFor(driverName);

            if (!driver)
            {
                qWarning("Could not find driver %s", qPrintable(driverName));
                return false;
            }

            device = driver->newDevice(info);

            if (device)
            {
                if (device->deviceId() == info.value("device_imsi"))
                {
                    qDebug("Device with SIM IMSI %s has been initialized ...", qPrintable(device->deviceId()));

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

                        connect(device, SIGNAL(connectionClosed()), this, SLOT(onConnectionClosed()));

                        QThread *thread = new QThread;
                        device->moveToThread(thread);
                        thread->start();
                    }
                    else
                    {
                        qWarning("Error phone numbers were not found for device %s", qPrintable(device->deviceId()));
                        result = false;
                    }
                }
                else
                {
                    qWarning("Configuration error for device with SIM IMSI %s", qPrintable(info.value("device_imsi")));
                    result = false;
                }
            }
            else
            {
                qWarning("Cannot create instance for device: %s", qPrintable(info.value("device_id")));
                result = false;
            }
        }

        return result;
    }

    bool DeviceManager::deleteDevice(const QString &deviceId)
    {
        ulong result(true);

        IDevice *deviceToDelete = deviceForId(deviceId);

        if (deviceToDelete)
        {            
//            deviceToDelete->deleteLater();
        }
        else
        {
            result = false;
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

    void DeviceManager::onConnectionClosed()
    {
        IDevice *device = (IDevice *)sender();
        if (device)
        {
            qWarning("Connection with device %s has been closed", qPrintable(device->deviceId()));
            delete device;
        }
    }

    DeviceManager::DeviceManager()
    {
        SystemConfig *config = SystemConfig::instance();

        //Creating Serial Devices
        foreach (DeviceInfo devInfo, config->devicesInfo())
        {
            if (devInfo.isEnabled())
            {
                qDebug(">> Device with ISMSI %s has lost connection", qPrintable(devInfo.value("device_id")));
                createDevice(devInfo);
            }
        }
    }

    DeviceManager::~DeviceManager()
    {

    }
}
