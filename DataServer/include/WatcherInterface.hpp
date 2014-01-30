#ifndef WATCHERINTERFACE_HPP
#define WATCHERINTERFACE_HPP

#include <QString>

#include "IDevice.hpp"
#include "DataStructures/WatcherInfo.hpp"

class WatcherInterface
{
    public:
        virtual IDevice *newWatcher(const WatcherInfo &info) = 0;

        virtual QString wactherName() const  = 0;
        virtual QString description() const  = 0;
};


Q_DECLARE_INTERFACE(WatcherInterface, "Gateway.WatcherInterface")


#endif // WATCHERINTERFACE_HPP
