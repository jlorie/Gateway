#ifndef REMOTESTORAGE_HPP
#define REMOTESTORAGE_HPP

#include <include/IMessage.hpp>
#include <include/DataStructures/MainInfo.hpp>
#include <include/DataStructures/MessageList.hpp>
#include <common/NetworkManager.hpp>
#include <common/CommonErrors.hpp>

#include <QObject>
#include <QMap>

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
        MainInfo *_config;

        bool _waitingResponse;
    };
}

#endif // REMOTESTORAGE_HPP
