#ifndef SYSTEMENGINE_HPP
#define SYSTEMENGINE_HPP

#include <QObject>
#include <include/IMessage.hpp>
#include <watcher/HttpWatcher.hpp>
#include <DeviceManager.hpp>

namespace Gateway
{
    using Watcher::HttpWatcher;

    class SystemEngine : public QObject
    {
        Q_OBJECT
    public:
        SystemEngine();

    public slots:
        void redirectMessage(const IMessage *message);

    private:
        void registerWatcher();

    private:
        HttpWatcher *_defaultWatcher;
        IWatcher *_watcher;

        qlonglong _lastId;
    };

}
#endif // SYSTEMENGINE_HPP
