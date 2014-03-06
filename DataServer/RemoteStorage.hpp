#ifndef REMOTESTORAGE_HPP
#define REMOTESTORAGE_HPP

#include <include/IMessage.hpp>
#include <include/DataStructures/MainInfo.hpp>
#include <include/DataStructures/MessageList.hpp>
#include <network/NetworkManager.hpp>

#include <QObject>
#include <QMap>
#include <QByteArray>

namespace Gateway
{
    class RemoteStorage : public QObject
    {
        Q_OBJECT
    public:
        static bool initialize();
        static RemoteStorage *instance();
        static void destroyInstance();

        MessageList pendingMessages();

    public slots:
        void dispatchMessage(const IMessage *message);
        void notifyMessageStatus(qlonglong messageId, MessageStatus status);

    private slots:
        void requestFinished(QNetworkReply *reply);
        void authenticate(QNetworkReply *reply, QAuthenticator *authenticator);

    private:
        RemoteStorage();

    private:
        static RemoteStorage *_instance;
        NetworkManager _networkManager;

        QByteArray _lastNetworkResponse;
    };
}

#endif // REMOTESTORAGE_HPP
