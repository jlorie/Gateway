#ifndef WATCHERMANAGER_HPP
#define WATCHERMANAGER_HPP

#include <include/WatcherInterface.hpp>

#include <QObject>
#include <QList>

namespace Gateway
{

    typedef QList<WatcherInterface *> WatcherLibraries;

    class WatcherManager : public QObject
    {
        Q_OBJECT
    public:
        explicit WatcherManager();

        static bool initialize();
        static WatcherManager *instance();
        static void destroyInstance();

        IWatcher *activeWatcher();
        void setWatcher(const QString &watcherName);

    private:
        void registerWatcher();

    private:
        static WatcherManager *_instance;
        WatcherLibraries _libraries;

        IWatcher *_activeWatcher;
    };

}
#endif // WATCHERMANAGER_HPP
