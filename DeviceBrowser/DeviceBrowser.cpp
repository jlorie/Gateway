#include "DeviceBrowser.hpp"

#include <QDebug>
#include <iostream>

DeviceBrowser::DeviceBrowser()
{
    _settings = new QSettings("Cubania Team", "Gateway");
}

DeviceBrowser::~DeviceBrowser()
{
    foreach (GSMDeviceInfo *devInfo, _devicesInfo.values())
    {
        delete devInfo;
    }
    _devicesInfo.clear();

    foreach (GSMDeviceInfo *devInfo, _devicesToAdd)
    {
        delete devInfo;
    }
    _devicesToAdd.clear();

    foreach (GSMDeviceInfo *devInfo, _devicesToRemove)
    {
        delete devInfo;
    }
    _devicesToRemove.clear();

    delete _settings;

}

void DeviceBrowser::sync()
{
    DeviceInfoList connected(infoList());
    foreach (QString devId, _devicesInfo.keys())
    {
        bool found(false);
        for (int i = 0; i < connected.size() && !found; ++i)
        {
            GSMDeviceInfo *devInfo(connected.at(i));
            if ( devId == devInfo->deviceId)
            {
                //update portname
                GSMDeviceInfo *tmp(_devicesInfo.value(devId));
                tmp->devicePort = devInfo->devicePort;
                tmp->isEnabled = true;

                //pop devices from connected list
                delete devInfo;
                connected.removeAt(i);

                found = true;
            }
        }

        if (!found)
        {
            _devicesToRemove.append(_devicesInfo.value(devId));
        }
    }

    foreach (GSMDeviceInfo *devInfo, connected)
    {
        _devicesToAdd.append(devInfo);
    }

    print();
}

void DeviceBrowser::loadSettings()
{
    int size = _settings->beginReadArray("Devices");
    for (int index = 0; index < size; ++index)
    {
        _settings->setArrayIndex(index);

        GSMDeviceInfo *devInfo = new GSMDeviceInfo;
        {
            devInfo->deviceId = _settings->value("deviceId").toString();
            devInfo->devicePort = _settings->value("devicePort").toString();
            devInfo->manufacturer = _settings->value("manufacturer").toString();
            devInfo->model = _settings->value("model").toString();
            devInfo->isEnabled = _settings->value("isEnabled").toBool();
        }

        _devicesInfo.insert(devInfo->deviceId, devInfo);
    }
    _settings->endArray();
}

void DeviceBrowser::saveSettings()
{
    _settings->beginWriteArray("Devices");

    int index(0);
    foreach (GSMDeviceInfo *devInfo, _devicesInfo.values())
    {
        _settings->setArrayIndex(index);
        _settings->setValue("device_id", devInfo->deviceId);
        _settings->setValue("serial_port", devInfo->devicePort);
        _settings->setValue("device_manufacturer", devInfo->manufacturer);
        _settings->setValue("device_model", devInfo->model);
        _settings->setValue("device_enabled", devInfo->isEnabled);
        _settings->setValue("driver_name", devInfo->deviceDriver);

        index++;
    }
    _settings->endArray();
}

DeviceInfoList DeviceBrowser::infoList()
{
    DeviceInfoList result;
    foreach (QSerialPortInfo portInfo, QSerialPortInfo::availablePorts())
    {
        GSMDeviceInfo *devInfo =  new GSMDeviceInfo;
        {
            //deviceId: productId|vendorId
            devInfo->deviceId = QString::number(portInfo.productIdentifier());
            devInfo->deviceId += QString("|") + QString::number(portInfo.vendorIdentifier());

            devInfo->devicePort = portInfo.portName();
            devInfo->manufacturer = portInfo.manufacturer();
            devInfo->model = portInfo.description();
        }

        result.append(devInfo);
    }

    return result;
}

void DeviceBrowser::print()
{

    if (!_devicesToAdd.isEmpty())
    {
        qDebug("New devices found:");
        qDebug("==================");

        foreach (GSMDeviceInfo *devInfo, _devicesToAdd)
        {
            qDebug() << "   device_id: " << devInfo->deviceId;
            qDebug() << "   serial_port: " << devInfo->devicePort;
            qDebug() << "   device_manufacturer: " << devInfo->manufacturer;
            qDebug() << "   device_model: " << devInfo->model;

            devInfo->isEnabled = true;
            devInfo->deviceDriver = QString("GenericGSMDriver");
            qDebug() << "   device_driver (by default): " << devInfo->deviceDriver;
            qDebug("\n");

            _devicesInfo.insert(devInfo->deviceId, devInfo);
        }
    }


    if (!_devicesToRemove.isEmpty())
    {
        qDebug("This devices are not longer connected and wil be disabled: ");
        qDebug("===========================================================");

        foreach (GSMDeviceInfo *devInfo, _devicesToRemove)
        {
            qDebug() << "   device_id: " << devInfo->deviceId;
            qDebug() << "   device_manufacturer: " << devInfo->manufacturer;
            qDebug() << "   device_model: " << devInfo->model;
            qDebug() << "   device_driver: " << devInfo->deviceDriver;
            qDebug("\n");

            devInfo->devicePort = QString();
            devInfo->isEnabled = false;
        }
    }

}
