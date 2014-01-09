#include "DeviceManager.hpp"

#include <QDebug>
#include <QThread>
#include <QDir>
#include <QPluginLoader>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <include/CommonErrors.hpp>
#include <include/IDevice.hpp>
#include <Storage.hpp>
#include <common/Rule.hpp>
#include <common/SMS.hpp>

const QString drivers_path("/home/lorie/workspace/My Projects/_qt-builds/build-gateway-Desktop-Debug/libs");

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
            bool found(false);

            QString driverName = info.value(QString("driver_name"), QString("GSMDriver"));
            foreach (DriverInterface *driver, _driverLibraries)
            {
                if (driver->driverName() == driverName)
                {
                    device = driver->newDevice(info);
                    found = true;
                    break;
                }
            }        

            if (device)
            {
                qDebug("Device %s initialized ...", qPrintable(info.value(QString("device_name"), QString("Unknown"))));

                _devices.append(device);
                foreach (IPhoneNumber *phoneNumber, device->phoneNumbers())
                {
                    qDebug("New phone number (%s) has been registered", qPrintable(phoneNumber->number()));
                    _numbers.append(phoneNumber);
                }


                qRegisterMetaType<IMessage *>("IMessage *");
                foreach (IPhoneNumber *phoneNumber, device->phoneNumbers())
                {
                    connect(phoneNumber, SIGNAL(newMessageReceived(const IMessage*)), this, SLOT(redirectSMS(const IMessage *)));
                    connect(phoneNumber, SIGNAL(messageSent(const IMessage*)), this, SLOT(messageSentNotification(const IMessage*)));
                }

                QThread *thread = new QThread;
                device->moveToThread(thread);
                thread->start();


//                IPhoneNumber *number = device->phoneNumbers().first();
//                SMS *message = new SMS();
//                {
//                    message->setTo(QString("+13852157548"));
//                    message->setBody("Hello world");
//                }

//                number->sendMessage((IMessage *)message);
            }
            else
            if (!found)
            {
                qWarning("Cannot find driver for device: %s", qPrintable(info.value("device_name")));
                result = Error::errDeviceNotFound;
            }
            else
            {
                qWarning("Cannot create instance for device: %s", qPrintable(info.value("device_name")));
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
            qDebug("Incomming message ...");
            qDebug(">>    from: %s", qPrintable(message->from()));
            qDebug(">>    to: %s", qPrintable(message->to()));
            qDebug(">>    body: %s", qPrintable(message->body()));

            Storage *storage = Storage::instance();
            IRule *redirectRule (storage->ruleFor(new Rule(message->from(), message->to())));
            if (redirectRule)
            {
                qDebug(">> Redirecting message to: %s", qPrintable(redirectRule->from()));

                //Find from number
                {
                    IPhoneNumber *fromNumber = 0;
                    foreach (IPhoneNumber *number, _numbers)
                    {
                        if (number->number() == redirectRule->from())
                            fromNumber = number;
                    }

                    if (fromNumber)
                    {
                        SMS *forwardMessage = new SMS();
                        {
                            forwardMessage->setTo(redirectRule->to());
                            forwardMessage->setBody(message->body());
                        }

                        fromNumber->sendMessage((IMessage *)forwardMessage);
                    }
                }
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
        loadDrivers(drivers_path);
//        browseSerialDevices();
        //-------------------------------------------------------

//        QString username("AC5d198c28d93f4ae9912408c0bffc47c2");
//        QString password("2260cd6a2f4f4145a3f2a73d42b3d472");

//        _webapi = new TelAPI(username, password, this);
    }

    DeviceManager::~DeviceManager()
    {
        foreach (DriverInterface *driver, _driverLibraries)
        {
            delete driver;
        }
    }

    void DeviceManager::loadDrivers(const QString &drivers_path)
    {
        QDir driversDir(drivers_path);
        foreach (QString fileName, driversDir.entryList(QDir::Files))
        {
            QString canonicalPath = driversDir.canonicalPath() + QDir::separator() + fileName;

            QPluginLoader loader(canonicalPath);
            QObject *library = loader.instance();
            if (library)
            {
                DriverInterface *driver = qobject_cast<DriverInterface *>(library);
                if (driver)
                {
                    qDebug("Registering driver: %s", qPrintable(driver->driverName()));
                    qDebug(">>    %s", qPrintable(driver->description()));

                    _driverLibraries.append(driver);
                }
            }
            else
            {
                QString error = loader.errorString();
                error = error.mid(error.lastIndexOf(".so:") + 4);
                qWarning("Cannot load driverLibrary %s: %s", qPrintable(fileName), qPrintable(error.toLatin1()));
            }
        }
    }

    void DeviceManager::messageSentNotification(const IMessage *message)
    {
        qDebug("Message sent to %s", qPrintable(message->to()));
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
