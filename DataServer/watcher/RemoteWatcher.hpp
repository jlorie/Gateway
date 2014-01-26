#ifndef REMOTEWATCHER_HPP
#define REMOTEWATCHER_HPP

#include <QObject>
#include <include/IMessage.hpp>

namespace Gateway
{

    class RemoteWatcher : public QObject
    {
        Q_OBJECT
    public:
        explicit RemoteWatcher(QObject *parent = 0):
            QObject(parent)
        {}

    signals:
        void messageReceived(const IMessage *message);
        void messageSent(uint messageId);
        void messageRefused(uint messageId);
    };

}
#endif // REMOTEWATCHER_HPP
