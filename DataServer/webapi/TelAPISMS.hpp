#ifndef TELAPISMS_HPP
#define TELAPISMS_HPP

#include <QNetworkAccessManager>
#include <QObject>
#include <QStringList>

class TelAPI : public QObject
{
    Q_OBJECT
public:
    TelAPI(const QString &userName, const QString &password, QObject *parent = 0);
    ~TelAPI();

    virtual QString sendSMS(const QString &from, const QString &to, const QString &message);
    virtual QStringList listSMS(const QMap<QString, QString> &filter);

public slots:
    void serviceRequestFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager _networkManager;    
    QString _userName;
    QString _password;
    QString _authorizationHeader;
};

#endif // TELAPISMS_HPP
