#include "DeviceManager.hpp"

#include <QDebug>
#include <QThread>
#include <QFile>
#include <QPluginLoader>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <include/CommonErrors.hpp>
#include <include/IDevice.hpp>
#include <Storage.hpp>
#include <common/Rule.hpp>

const QString gsm_driver_library("/home/lorie/workspace/My Projects/_qt-builds/build-gateway-Desktop-Debug/libs/libGSMDriver.so");

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
            foreach (QObject *driver, _driverLibraries)
            {
                device = qobject_cast<IDevice *>(driver);
                if (device)
                    break;
            }        
            if (!device)
            {
                qWarning("Could not find driver for device %s", qPrintable(info.value("device_name")));
            }
            else
            if (device->initialize(info) == Error::OK)
            {
                qWarning() << "Device " <<  info.value(QString("device_name"), QString("Unknown")) << "initialized ...";

                _devices.append(device);
                _numbers.append(device->phoneNumbers());

                qRegisterMetaType<IMessage *>("IMessage *");
                foreach (IPhoneNumber *phoneNumber, device->phoneNumbers())
                {
                    connect(phoneNumber, SIGNAL(newMessageReceived(const IMessage*)), this, SLOT(redirectSMS(const IMessage *)));
                }

    //            QThread *thread = new QThread;
    //            device->moveToThread(thread);
    //            thread->start();
            }
            else
            {
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

    void DeviceManager::redirectSMS(const IMessage* message)
    {        
        if (message)
        {
            qDebug(">> Incomming message ...");
            qDebug("    from: %s", qPrintable(message->from()));
            qDebug("    to: %s", qPrintable(message->to()));
            qDebug("    body: %s", qPrintable(message->body()));
            // Contacts
//            QString twilioNumber("+13852157548");
//            QString osmarNumber("+17862180956");
//            QString telapiNumber("+15305765603");

    //        QString to(ruleFor(message->from()));
            Storage *storage = Storage::instance();
            IRule *redirectRule (storage->ruleFor(new Rule(message->from(), message->to())));
            if (redirectRule)
            {
                qDebug(">> Redirecting message ...");
                qDebug("    from: %s", qPrintable(redirectRule->from()));
                qDebug("    to: %s", qPrintable(redirectRule->to()));
                qDebug("    body: %s", qPrintable(message->body()));

                _webapi->sendSMS(redirectRule->from(), redirectRule->to(), message->body());
            }
            else
            {
                qWarning("No rule found for previous message");
            }
        }
        else
        {
            qWarning("No message found");
        }
    }

    DeviceManager::DeviceManager(QObject *parent) :
        QObject(parent)
    {
        // Loading gsm drivers
        {
            // TODO reconocer las bibliotecas dentro de un directorio
            QString fileName(gsm_driver_library);

            if (QFile::exists(fileName))
            {
                QPluginLoader loader(fileName);
                QObject *library = loader.instance();
                if (library)
                {
                    qDebug("Plugin found: %s", qPrintable(fileName));
                    _driverLibraries.append(library);
                }
                else
                {
                    qWarning("Could not open library: %s", qPrintable(fileName));
                }
            }
            else
            {
                qWarning("Could not find library %s", qPrintable(fileName));
            }
        }

        browseSerialDevices();
        //-------------------------------------------------------

        QString username("AC5d198c28d93f4ae9912408c0bffc47c2");
        QString password("2260cd6a2f4f4145a3f2a73d42b3d472");

        _webapi = new TelAPI(username, password, this);
    }

    DeviceManager::~DeviceManager()
    {
        foreach (QObject *library, _driverLibraries)
        {
            delete library;
        }
    }

    #include <QSerialPort>
    #include <QSerialPortInfo>

    void DeviceManager::browseSerialDevices()
    {
        qDebug(">> Browsing serial devices ...");

        foreach (QSerialPortInfo portInfo, QSerialPortInfo::availablePorts())
        {
            DeviceInfo devInfo;
            {
                devInfo.insert(QString("manufacturer"), portInfo.manufacturer());
                devInfo.insert(QString("device_type"), QString("serial"));
                devInfo.insert(QString("serial_port"), portInfo.portName());
                devInfo.insert(QString("device_name"), portInfo.portName());
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

}
