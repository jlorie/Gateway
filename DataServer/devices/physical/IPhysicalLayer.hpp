#ifndef IPHYSICAL_HPP
#define IPHYSICAL_HPP

#include <QString>
#include <QRegExp>

namespace Gateway
{
    class IPhysicalLayer
    {
    public:
        virtual ~IPhysicalLayer(){}

        virtual bool isConnected () const = 0;

        virtual bool disconnect () = 0;

        virtual bool open (ulong timeOut) = 0;

        virtual bool send (const QString &data) = 0;

        virtual bool receive (ulong timeOut, QString &data) = 0;

        virtual bool receiveUntil (char criteria, ulong maxSize, ulong timeOut, QString &data) = 0;

        virtual ulong error() const = 0;

        virtual QString errorString() const = 0;
    };
}

#endif // IPHYSICAL_HPP
