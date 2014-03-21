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
            if (!devInfo.isEnabled())
                continue;

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

            bool deviceCreated(false);

            QStringList serialPorts(availablePortsFor(info.value("device_id")));
            for (int i = 0; i < serialPorts.size() && !deviceCreated; ++i)
            {
                QString serialPort(serialPorts.at(i));
                info.insert("serial_port", serialPort);
                qDebug("Trying to connect on port (%s)", qPrintable(serialPort));

                device = driver->newDevice(info);

                if (device && device->deviceIMEI() == info.value("device_imei"))
                {
                    qDebug("Device with SIM IMEI %s has been initialized ...", qPrintable(device->deviceIMEI()));

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
                        _portsInUse.append(serialPort);
                        deviceCreated = true;
                    }
                    else
                    {
                        qWarning("No phone number found for device with IMEI: %s", qPrintable(info.value("device_imei")));

                        delete device;
                        deviceCreated = false;
                        break;
                    }

                }
                else
                {
                    qWarning("Phone IMEI not match ... deleting device");
                    delete device;
                }
            }

            if (!deviceCreated)
            {
                qWarning("No serial port has found for device with IMEI %s",
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
            delete device;
            qWarning("Connection with device imei %s has been closed, trying to reconnect", qPrintable(imei));

            SystemConfig *config = SystemConfig::instance();

            //Creating Serial Devices
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
    }

    DeviceManager::~DeviceManager()
    {

    }

    QStringList DeviceManager::availablePortsFor(const QString &deviceId) const
    {
        QStringList ports;
        QList<QSerialPortInfo> infos(QSerialPortInfo::availablePorts());

        // remove 0 at front
        QString trimmedId(deviceId);
        {
            while(trimmedId.at(0) == '0')
                trimmedId.remove(0, 1);
        }

        foreach (QSerialPortInfo info, infos)
        {
            QString devId = QString::number(info.vendorIdentifier(), 16)
                    + ":"
                    + QString::number(info.productIdentifier(), 16);

            if (trimmedId == devId) // el dispositivo esta libre
            {
                QString serialPort("/dev/" + info.portName()); // FIXME obtener la direccion multiplataforma
                ports.insert(info.productIdentifier(), serialPort);
            }
        }

        return ports;
    }
}
