#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <QtPlugin>
#include <QJsonObject>

#include <include/IDevice.hpp>
#include "NetworkManager.hpp"

class Device : public IDevice
{
    public:
        Device(const DeviceInfo &info);
        ~Device();
        bool initialize();
        qlonglong deviceId();

        NumberList phoneNumbers() const;

        DeviceClass type() const;

    private:
        bool queryPhoneNumbers();
        void handleQueryPhoneNumbers(const QJsonObject &reply);

    private:
        NumberList _numbers;
        NetworkManager *_networkManager;

        QString _username;
        QString _password;
};

#endif // DEVICE_HPP
