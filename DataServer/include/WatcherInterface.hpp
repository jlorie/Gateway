#ifndef WATCHERINTERFACE_HPP
#define WATCHERINTERFACE_HPP

#include <QString>

#include "IWatcher.hpp"
#include "DataStructures/WatcherInfo.hpp"

class WatcherInterface
{
    public:
        virtual IWatcher *newWatcher(WatcherInfo *info) = 0;

        virtual QString watcherName() const  = 0;
        virtual QString description() const  = 0;
};


Q_DECLARE_INTERFACE(WatcherInterface, "Gateway.WatcherInterface")


#endif // WATCHERINTERFACE_HPP
