#ifndef NUMBERINFO_HPP
#define NUMBERINFO_HPP

#include "../IDevice.hpp"

#include <QString>

class NumberInfo
{
public:
    NumberInfo(const QString &number, IDevice *device)
        :_number(number),_device(device)
    {}

    QString number() const
    {
        return _number;
    }

    IDevice *device() const
    {
        return _device;
    }

private:
    QString _number;
    IDevice *_device;
};

#endif // PHONENUMBER_HPP
