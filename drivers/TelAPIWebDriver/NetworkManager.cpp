#include "NetworkManager.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>

NetworkManager::NetworkManager(const QString &username, const QString &password, QObject *parent)
    :QObject(parent), _username(username), _password(password)
{
    _responseFormat = ResponseFormat::Json;

    connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    connect(&_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticate(QNetworkReply*,QAuthenticator*)));
}

NetworkManager::~NetworkManager()
{
    foreach (QNetworkReply *reply, _pendingReplys.values())
    {
        reply->close();
        reply->deleteLater();
    }

    _pendingReplys.clear();
}

qlonglong NetworkManager::post(const QUrl &url, const QByteArray &postData)
{
    QNetworkRequest request;
    {
        request.setUrl(withFormatResponse(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    }

    QNetworkReply *reply = _networkManager.post(request, postData);
    _pendingReplys.insert((qlonglong)reply, reply);

    return (qlonglong)reply;
}

qlonglong NetworkManager::get(const QUrl &url)
{
    QNetworkRequest request;
    {        
        request.setUrl(withFormatResponse(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    }

    QNetworkReply *reply = _networkManager.get(request);
    _pendingReplys.insert((qlonglong)reply, reply);

    return (qlonglong)reply;
}

QByteArray NetworkManager::waitForResponse(const qlonglong requestId)
{
    if (!_pendingReplys.contains(requestId))
    {
        qWarning("Reply for request %lld not available", requestId);
        return QByteArray();
    }

    QNetworkReply *reply(_pendingReplys.value(requestId, 0));
    if (!reply)
    {
        qWarning("Request %lld not found", requestId);
        return QByteArray();
    }

    //Waiting for response
    _waitLoop.exec();

    return _response;
}

void NetworkManager::requestFinished(QNetworkReply* reply)
{
    if (reply->error())
    {
        qWarning("Request error: %s", qPrintable(reply->errorString()));

        if (_waitLoop.isRunning())
            _waitLoop.quit();

        return;
    }

    const qlonglong requestId((qlonglong)reply);
    if (!_pendingReplys.contains(requestId))
    {
        qWarning("Reply for request %lld not available", requestId);
        return;
    }

    _pendingReplys.take(requestId);

    _response = reply->readAll();

    if (_waitLoop.isRunning())
        _waitLoop.quit();
    else
        emit responseReady(requestId, _response);

    reply->deleteLater();
}


void NetworkManager::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);

    authenticator->setUser(_username);
    authenticator->setPassword(_password);
}

QUrl NetworkManager::baseRequestUrl() const
{
    return QUrl(QString("https://api.telapi.com/v1/Accounts/%1").arg(_username));
}

void NetworkManager::setResponseFormat(ResponseFormat format)
{
    _responseFormat = format;
}

QUrl NetworkManager::withFormatResponse(const QUrl &url)
{
    QString format("");
    switch (_responseFormat)
    {
        case ResponseFormat::Json:
            format = QString(".json");
            break;
        case ResponseFormat::Xml:
            break;
        case ResponseFormat::Csv:
            format = QString(".csv");
            break;
        default:
            break;
    }

    QString result = url.toString();
    int pos = result.length();

    if (url.hasQuery())
        pos = result.indexOf('?');

    result.insert(pos, format);

    return QUrl(result);
}
