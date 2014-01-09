#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QMap>
#include <QEventLoop>

typedef QMap<qlonglong, QNetworkReply *> ReplyList ;

enum class ResponseFormat
{
    Json,
    Xml,
    Csv
};

class NetworkManager : public QObject
{
    Q_OBJECT
    public:
        NetworkManager(const QString &username, const QString &password, QObject *parent = 0);
        ~NetworkManager();

        /**
         * @return Retorna el id de la nueva solicitud
         */
        qlonglong post(const QUrl &url, const QByteArray &postData);
        qlonglong get(const QUrl &url);

        QByteArray waitForResponse(const qlonglong requestId);
        QUrl baseRequestUrl() const;

        void setResponseFormat(ResponseFormat format);

    signals:
        void responseReady(qlonglong requestId, QByteArray response);

    protected slots:
        void authenticate(QNetworkReply * reply, QAuthenticator * authenticator);
        void requestFinished(QNetworkReply *reply);

    private:        
        QUrl withFormatResponse(const QUrl &url);

    private:
        QString _username;
        QString _password;
        ResponseFormat _responseFormat;

        QString _authorizationHeader;

        QNetworkAccessManager _networkManager;
        ReplyList _pendingReplys;
        QEventLoop _waitLoop;
        QByteArray _response;
};



#endif // NETWORKMANAGER_HPP
