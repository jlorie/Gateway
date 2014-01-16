#include "DeviceManager.hpp"
#include "DriverManager.hpp"

#include <QDebug>
#include <QThread>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QDebug>

#include <include/CommonErrors.hpp>
#include <include/IDevice.hpp>
#include <Storage.hpp>
#include <common/Rule.hpp>
#include <common/SMS.hpp>


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

            QString driverName = info.value(QString("driver_name"), QString("GenericGSMDriver"));
            DriverInterface *driver = DriverManager::instance()->driverFor(driverName);
            device = driver->newDevice(info);

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
    }

    DeviceManager::~DeviceManager()
    {

    }

    void DeviceManager::messageSentNotification(const IMessage *message)
    {
        qDebug("Message sent to %s", qPrintable(message->to()));
    }
}
