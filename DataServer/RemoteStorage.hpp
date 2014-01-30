#ifndef REMOTESTORAGE_HPP
#define REMOTESTORAGE_HPP

#include <include/IMessage.hpp>

#include <QObject>
#include <QNetworkAccessManager>

namespace Gateway
{

    class RemoteStorage : public QObject
    {
        Q_OBJECT
    public:
        static bool initialize();
        static RemoteStorage *instance();

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
        QNetworkAccessManager _networkManager;

        QString _username;
        QString _password;

    };
}

#endif // REMOTESTORAGE_HPP
