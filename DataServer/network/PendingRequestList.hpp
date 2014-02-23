#ifndef PENDINGREQUESTLIST_HPP
#define PENDINGREQUESTLIST_HPP

#include <QNetworkRequest>
#include <QDir>
#include <QList>

namespace Gateway
{
    class PendingRequestList: public QList<QNetworkRequest>
    {
    public:
        PendingRequestList();
        ~PendingRequestList();

        QNetworkRequest head() const;
        void enqueue(const QNetworkRequest &request);
        QNetworkRequest dequeue();

    protected:
        virtual void load();
        virtual void save() const;

    private:
        QString generateId(const QNetworkRequest &requests) const;

    private:
        QDir _directory;
    };
}

#endif // PENDINGREQUESTLIST_HPP
