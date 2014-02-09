#ifndef REMOTESTORAGE_HPP
#define REMOTESTORAGE_HPP

#include <include/IMessage.hpp>
#include <include/DataStructures/MainInfo.hpp>
#include <include/DataStructures/MessageList.hpp>

#include <QObject>
#include <QMap>
#include <QPair>
#include <QNetworkAccessManager>

namespace Gateway
{
    typedef QPair<QNetworkRequest *, QByteArray *> Request;
    typedef QMap<qlonglong, Request *> PendingRequests;

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

        void resendFailedMessages();

    private:
        RemoteStorage();

    private:
        static RemoteStorage *_instance;
        QNetworkAccessManager _networkManager;
        MainInfo *_config;
        PendingRequests _failedRequests;

        bool _waitingResponse;
    };
}

#endif // REMOTESTORAGE_HPP
