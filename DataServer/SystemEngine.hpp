#ifndef SYSTEMENGINE_HPP
#define SYSTEMENGINE_HPP

#include <QObject>
#include <include/IMessage.hpp>
#include <include/IWatcher.hpp>
#include <DeviceManager.hpp>

namespace Gateway
{
    class SystemEngine : public QObject
    {
        Q_OBJECT
    public:
        SystemEngine();

    public slots:
        void redirectMessage(const IMessage *message);

    private:
        void registerWatcher();
        void setProxy();

    private:
        IWatcher *_watcher;
        qlonglong _lastId;
    };

}
#endif // SYSTEMENGINE_HPP
