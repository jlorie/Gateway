#include "DeviceManager.hpp"
#include "DriverManager.hpp"
#include "RemoteStorage.hpp"

#include <SystemConfig.hpp>
#include <include/IDevice.hpp>

#include <qserialportinfo.h>

#include <QDebug>
#include <QThread>

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
            if (devInfo.isEnabled())
            {
                createDevice(devInfo);
            }
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

            bool deviceCreated(false);
            QStringList serialPorts(availableSerialPorts());
            for (int i = 0; i < serialPorts.size() && !deviceCreated; ++i)
            {
                info.insert("serial_port", serialPorts.at(i));
                device = driver->newDevice(info);

                if (device && device->deviceId() == info.value("device_imsi"))
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

                        // getting pending messages from device
                        foreach (IMessage *message, device->pendingMessages())
                        {
                            emit newMessageReceived(message);
                        }

                        QThread *thread = new QThread;
                        device->moveToThread(thread);
                        thread->start();

                        _devices.append(device);
                    }

                    deviceCreated = true;
                }
            }

            if (!deviceCreated)
            {
                qWarning("No serial port has found for device with IMSI %s",
                         qPrintable(info.value("device_imsi")));
            }
            result = deviceCreated;
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
            QString imsi = device->deviceId();
            delete device;
            qWarning("Connection with device %s has been closed, trying to reconnect", qPrintable(imsi));

            SystemConfig *config = SystemConfig::instance();

            //Creating Serial Devices
            foreach (DeviceInfo devInfo, config->devicesInfo())
            {
                if (devInfo.isEnabled() && imsi == devInfo.value("device_imsi"))
                {
                    createDevice(devInfo);
                }
            }
        }
    }

    DeviceManager::DeviceManager()
    {
    }

    DeviceManager::~DeviceManager()
    {

    }

    QStringList DeviceManager::availableSerialPorts() const
    {
        QMap<uint, QString> ports;
        QList<QSerialPortInfo> infos(QSerialPortInfo::availablePorts());

        foreach (QSerialPortInfo info, infos)
        {
            if (ports.find(info.productIdentifier()) == ports.end())
            {
                ports.insert(info.productIdentifier(), "/dev/" + info.portName()); // FIXME obtener la direccion multiplataforma
            }
        }

        return ports.values();
    }
}
