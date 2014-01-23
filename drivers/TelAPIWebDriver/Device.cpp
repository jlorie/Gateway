#include <include/CommonErrors.hpp>

#include <QNetworkRequest>
#include <QByteArray>
#include <QUrl>
#include <QEventLoop>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "Device.hpp"
#include "PhoneNumber.hpp"
#include "Message.hpp"

const ulong TimeOut = 3000;

Device::Device(const DeviceInfo &info)
{
    _username = info.value(QString("device_user"), QString("none"));
    _password = info.value(QString("device_password"), QString("none"));

    _networkManager = new NetworkManager(_username, _password, this);
}

Device::~Device()
{
    delete _networkManager;
}

bool Device::initialize()
{
    return queryPhoneNumbers();
}

qlonglong Device::deviceId()
{
    qlonglong serialNumber(2);
    return serialNumber;
}

NumberList Device::phoneNumbers() const
{
    return _numbers;
}

bool Device::queryPhoneNumbers()
{
    QUrl query(_networkManager->baseRequestUrl().toString().append(QString("/IncomingPhoneNumbers")));

    qlonglong requestId(_networkManager->get(query));
    QByteArray reply(_networkManager->waitForResponse(requestId));

    QJsonDocument jsonResponse = QJsonDocument::fromJson(((QString)reply).toUtf8());

    if (!jsonResponse.isObject())
    {
        qWarning("Empty reply!!!");
        return false;
    }

    handleQueryPhoneNumbers(jsonResponse.object());
    return true;
}

void Device::handleQueryPhoneNumbers(const QJsonObject &reply)
{    
    if (reply.find(QString("incoming_phone_numbers")) != reply.end())
    {
        QJsonArray incomingPhoneNumbers = reply.value(QString("incoming_phone_numbers")).toArray();

        foreach (const QJsonValue & value, incomingPhoneNumbers)
        {
            QJsonObject obj = value.toObject();
            QJsonObject::iterator it = obj.find(QString("phone_number"));

            if (it != obj.end())
            {
                QString phoneNumber = it.value().toString();
                _numbers.append(new PhoneNumber(_networkManager, phoneNumber));

                //TODO evitar que se dupliquen los numeros telefonicos
            }
        }
    }
}
