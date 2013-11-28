#include "TelAPISMS.hpp"

#include <QDebug>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>

TelAPI::TelAPI(const QString &userName, const QString &password, QObject *parent)
    :QObject(parent), _userName(userName), _password(password)
{
    connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)));
    connect(&_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)));

    QString concatenated = userName + ":" + password;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    _authorizationHeader = "Basic " + data;
}

TelAPI::~TelAPI()
{

}

QString TelAPI::sendSMS(const QString &from, const QString &to, const QString &message)
{
    qDebug() << "Sending sms";

    QNetworkRequest request;
    request.setUrl(QUrl(QString("https://api.telapi.com/v1/Accounts/%1/SMS/Messages.json").arg(_userName)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", _authorizationHeader.toLocal8Bit());

    QByteArray postData;
    postData.append(QByteArray("To=")).append(to);
    postData.append(QByteArray("&From=")).append(from);
    postData.append(QByteArray("&Body=")).append(message);

    _networkManager.post(request, postData);

    return QString();
}

QStringList TelAPI::listSMS(const QMap<QString, QString> &filter)
{
    qDebug() << "Listing SMS";

    QNetworkRequest request;
    QUrl url(QString("https://api.telapi.com/v1/Accounts/%1/SMS/Messages.json").arg(_userName));

    // estableciendo los parametros para filtrar los mensajes
    QUrlQuery query;
    foreach (QString key, filter.keys())
    {
        qDebug() << key << ": " << filter[key];

        query.addQueryItem(key, filter[key]);
    }

    url.setQuery(query);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", _authorizationHeader.toLocal8Bit());

    _networkManager.get(request);

    qDebug() << "Url" << url.toString();

    return QStringList();
}

void TelAPI::serviceRequestFinished(QNetworkReply *reply)
{
    qDebug() << reply->readAll()  << reply->errorString();
}
