#ifndef ABSTRACTGSMDEVICE_HPP
#define ABSTRACTGSMDEVICE_HPP

#include "DeviceInfo.hpp"
#include <common/SMS.hpp>

#include <QObject>

namespace Gateway
{
    class AbstractGSMDevice : public QObject
    {
        Q_OBJECT
    public:
        explicit AbstractGSMDevice(const DeviceInfo &info, QObject *parent = 0);
        ~AbstractGSMDevice();

        virtual bool initialize();

        DeviceInfo info() const;

    signals:
        void newSMSReceived(const SMS &sms);        

    protected:
        DeviceInfo _deviceInfo;        
    };
}
#endif // ABSTRACTGSMDEVICE_HPP
