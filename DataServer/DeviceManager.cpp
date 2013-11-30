#include "DeviceManager.hpp"
#include <common/CommonErrors.hpp>
#include <devices/SerialGSMDevice.hpp>

#include <QDebug>
#include <QThread>

#include <storage/LocalStorage.hpp>

namespace Gateway
{
    DeviceManager * DeviceManager::_instance = 0;

    void DeviceManager::initialize()
    {
        if (!_instance)        
            _instance = new DeviceManager;
    }

    DeviceManager *DeviceManager::instance()
    {
        return _instance;
    }

    void DeviceManager::destroyInstance()
    {
        if (_instance)
            delete _instance;

        _instance = 0;
    }

    ulong DeviceManager::createDevice(const DeviceInfo &info)
    {
        ulong result = Errors::OK;        

        // TODO implementar una fabrica de Device
        AbstractGSMDevice *device;
        if (info.protocol == "serial")
        {
            device = new SerialGSMDevice(info);
        }

        if (device->initialize())
        {
            qWarning() << "Device " <<  device->info().name << "initialized ...";

            _devices.append(device);

            qRegisterMetaType<SMS>("SMS");
            connect(device, SIGNAL(newSMSReceived(SMS)), this, SLOT(redirectSMS(SMS)));

            QThread *thread = new QThread;
            device->moveToThread(thread);
            thread->start();
        }
        else
        {
            delete device;
            result = Errors::ErrorInitializingDevice;
        }

        return result;
    }

    ulong DeviceManager::deleteDevice(const qlonglong deviceID)
    {
        ulong result = Errors::OK;

        AbstractGSMDevice *deviceToDelete = deviceForId(deviceID);

        if (deviceToDelete)
        {            
            deviceToDelete->deleteLater();
        }
        else
        {
            result = Errors::DeviceNotFound;
        }

        return result;
    }

    AbstractGSMDevice *DeviceManager::deviceForId(qlonglong deviceID) const
    {
        AbstractGSMDevice *result = 0;

        foreach (AbstractGSMDevice *device, _devices)
        {
            if (device->info().deviceID == deviceID)
            {
                result = device;
                break;
            }
        }

        return result;
    }

    void DeviceManager::redirectSMS(const SMS &sms)
    {        
        qDebug() << "Redirecting sms ...";
        qDebug() << sms.toString();

        // Contacts
//        QString twilioNumber("+13852157548");
//        QString osmarNumber("+17862180956");
        QString telapiNumber("+15305765603");

        QString to(ruleFor(sms.from()));
        if (!to.isEmpty())
        {
            _webapi->sendSMS(telapiNumber, to, sms.body());
        }
    }

    DeviceManager::DeviceManager(QObject *parent) :
        QObject(parent)
    {
        browseSerialDevices();

        QString username("AC5d198c28d93f4ae9912408c0bffc47c2");
        QString password("2260cd6a2f4f4145a3f2a73d42b3d472");

        _webapi = new TelAPI(username, password, this);
    }

    #include <QSerialPort>
    #include <QSerialPortInfo>

    void DeviceManager::browseSerialDevices()
    {
        qWarning(">> Browsing serial devices ...");

        foreach (QSerialPortInfo portInfo, QSerialPortInfo::availablePorts())
        {
            DeviceInfo devInfo;
            {
                devInfo.deviceID = (qlonglong)1; // FIXME
                devInfo.protocol = QString("serial");
                devInfo.manufacturer = portInfo.manufacturer();
                devInfo.name = portInfo.portName();
            }

//            qDebug() << "\nPort:" << portInfo.portName();
//            qDebug() << "Location:" << portInfo.systemLocation();
//            qDebug() << "Description:" << portInfo.description();
//            qDebug() << "Manufacturer:" << portInfo.manufacturer();
//            qDebug() << "Vendor Identifier:" << (portInfo.hasVendorIdentifier() ? QByteArray::number(portInfo.vendorIdentifier(), 16) : QByteArray());
//            qDebug() << "Product Identifier:" << (portInfo.hasProductIdentifier() ? QByteArray::number(portInfo.productIdentifier(), 16) : QByteArray());
//            qDebug() << "Busy:" << (portInfo.isBusy() ? QObject::tr("Yes") : QObject::tr("No"));

//            QSerialPort *port = new QSerialPort(portInfo);
//            if (port->open(QIODevice::ReadWrite)) {
//                qDebug() << "Baud rate:" << port->baudRate();
//                qDebug() << "Data bits:" << port->dataBits();
//                qDebug() << "Stop bits:" << port->stopBits();
//                qDebug() << "Parity:" << port->parity();
//                qDebug() << "Flow control:" << port->flowControl();
//                qDebug() << "Read buffer size:" << port->readBufferSize();
//                port->close();
//            } else {
//                qDebug() << "Unable to open port, error code" << port->error();
//            }

            createDevice(devInfo);
        }
    }

    QString DeviceManager::ruleFor(const QString &from)
    {
        LocalStorage *storage = LocalStorage::instance();
        foreach (IRule *rule, storage->rules())
        {
            if (rule->from() == from)
                return rule->to();
        }

        return QString();
    }

}
