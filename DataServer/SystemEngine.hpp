#ifndef SYSTEMENGINE_HPP
#define SYSTEMENGINE_HPP

#include <QObject>
#include <include/IMessage.hpp>
#include "watcher/RemoteWatcher.hpp"
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
        RemoteWatcher *_watcher;
    };

}
#endif // SYSTEMENGINE_HPP
