#ifndef REQUESTINFO_HPP
#define REQUESTINFO_HPP

#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace Gateway
{
    class RequestInfo
    {
    public:
        RequestInfo(QNetworkAccessManager::Operation op,QIODevice *outgoingData)
            :_op(op), _outgoingData(outgoingData)
        {}

        QNetworkAccessManager::Operation operation() const
        {
            return _op;
        }

        QIODevice *outgoingData() const
        {
            return _outgoingData;
        }

    private:
        QNetworkAccessManager::Operation _op;
        QIODevice *_outgoingData;
    };
}
#endif // REQUESTINFO_HPP
