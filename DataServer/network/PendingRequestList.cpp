#include "PendingRequestList.hpp"

#include <SystemConfig.hpp>

#include <QDir>
#include <QFile>
#include <QCryptographicHash>

namespace Gateway
{
    const int AttrPostData = 1001;

    PendingRequestList::PendingRequestList()
    {
        QString path = SystemConfig::instance()
                ->value("Main/pending_request_path", "/var/gateway/pending_requests/")
                .toString();

        _directory.setCurrent(path);
        load();
    }

    PendingRequestList::~PendingRequestList()
    {
        save();
    }

    QNetworkRequest PendingRequestList::head() const
    {
        return first();
    }

    void PendingRequestList::enqueue(const QNetworkRequest &request)
    {
        append(request);
        save();
    }

    QNetworkRequest PendingRequestList::dequeue()
    {
        Q_ASSERT(!isEmpty());

        QNetworkRequest request(this->takeFirst());
        QString fileName = QString("%1/pending_request_%2")
                .arg(_directory.path())
                .arg(generateId(request));

        if (QFile::exists(fileName))
            QFile::remove(fileName);

        return request;
    }

    void PendingRequestList::load()
    {
        foreach (QString fileName, _directory.entryList(QDir::Files|QDir::NoDotAndDotDot))
        {
            fileName = _directory.absoluteFilePath(fileName);
            QSettings settings(fileName, QSettings::IniFormat);
            QString url(settings.value("url").toString());
            QString postData(settings.value("post_data").toString());

            QNetworkRequest request;
            {
                request.setUrl(QUrl(url));
                request.setAttribute((QNetworkRequest::Attribute)AttrPostData, postData);
            }

            append(request);
        }

    }

    void PendingRequestList::save() const
    {
        for (int i = 0; i < size(); ++i)
        {
            const QNetworkRequest &request(at(i));

            QString fileName = QString("%1/pending_request_%2")
                    .arg(_directory.path())
                    .arg(generateId(request));

            QString url = request.url().toString();
            QString postData(request
                                .attribute((QNetworkRequest::Attribute)AttrPostData)
                                .toString());

            if (!QFile::exists(fileName))
            {
qDebug("----> %s", qPrintable(fileName)); //NOTE
                QSettings settings(fileName, QSettings::IniFormat);
                settings.setValue("url", url);
                settings.setValue("post_data", postData);
            }
        }

    }

    QString PendingRequestList::generateId(const QNetworkRequest &request) const
    {
        QString url = request.url().toString();
        QString postData(request
                            .attribute((QNetworkRequest::Attribute)AttrPostData)
                            .toString());

        QByteArray data (QString(url + postData).toLatin1());
        QByteArray id = QCryptographicHash::hash(data, QCryptographicHash::Md5);

        return QString(id.toHex().data());
    }
}
