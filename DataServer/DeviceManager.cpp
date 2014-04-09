#include "DeviceManager.hpp"
#include "DriverManager.hpp"
#include "RemoteStorage.hpp"

#include <SystemConfig.hpp>
#include <include/IDevice.hpp>

#include <qserialportinfo.h>

#include <QDebug>
#include <QThread>
#include <QDir>

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

    void DeviceManager::loadDevices()
    {
        SystemConfig *config = SystemConfig::instance();

        //Creating Serial Devices
        foreach (DeviceInfo devInfo, config->devicesInfo())
        {
            createDevice(devInfo);
        }
    }

    bool DeviceManager::createDevice(DeviceInfo info)
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

            qDebug("Trying to connect on port (%s)", qPrintable(info.value("serial_port")));
            device = driver->newDevice(info);
            if (!device)
            {
                qWarning("Could not create instance for device with IMEI %s", qPrintable(device->deviceIMEI()));
                return false;
            }

            if (device->deviceIMEI() == info.value("device_imei"))
            {
                qDebug("Device with IMEI %s has been initialized ...", qPrintable(device->deviceIMEI()));

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

                    // getting pending messages from device
                    foreach (const IMessage *message, device->pendingMessages())
                    {
                        emit newMessageReceived(message);
                    }

                    QThread *thread = new QThread;
                    device->moveToThread(thread);
                    thread->start();

                    _devices.append(device);
                }
                else
                {
                    qWarning("No phone number found for device with IMEI: %s", qPrintable(info.value("device_imei")));
                    delete device;

                    result = false;
                }
            }
            else
            {
                qWarning("Phone IMEI not match ... deleting device");
                delete device;

                result = false;
            }
        }

        return result;
    }

    bool DeviceManager::deleteDevice(const QString &imei)
    {
        bool result(false);

        for (int i = 0; i < _devices.size() && !result; ++i)
        {
            IDevice *device(_devices.at(i));
            if (device && device->deviceIMEI() == imei)
            {
                _devices.removeAt(i);

                device->thread()->quit();
                device->deleteLater();

                result = true;
            }
        }

        if (result)
        {
            qDebug("Device with IMEI %s has been deleted", qPrintable(imei));
        }

        return result;
    }

    IDevice *DeviceManager::deviceForId(const QString &deviceId) const
    {
        IDevice *result = 0;

        foreach (IDevice *device, _devices)
        {
            if (device->deviceIMEI() == deviceId)
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
            QString imei = device->deviceIMEI();
            qWarning("Connection with device imei %s has been closed, trying to reconnect", qPrintable(imei));

            deleteDevice(imei);

            SystemConfig *config = SystemConfig::instance();
            foreach (DeviceInfo devInfo, config->devicesInfo())
            {
                if (devInfo.isEnabled() && imei == devInfo.value("device_imei"))
                {
                    createDevice(devInfo);
                }
            }
        }
    }

    DeviceManager::DeviceManager()
    {
        SystemConfig *config(SystemConfig::instance());

        connect(config, SIGNAL(newDeviceFound(DeviceInfo)), this, SLOT(createDevice(DeviceInfo)));
        connect(config, SIGNAL(deviceRemoved(QString)), this, SLOT(deleteDevice(QString)));
    }

    DeviceManager::~DeviceManager()
    {

    }
}
