#ifndef INCOMINGSMSRECORD_HPP
#define INCOMINGSMSRECORD_HPP

#include "AbstractRecord.hpp"

#include <QRegExp>
#include <QString>
#include <QDate>

namespace Gateway
{
    class IncomingSMSRecord : public AbstractRecord
    {
    public:
        IncomingSMSRecord()
        {}

        RecordType type() const
        {
            return recInconmingMessage;
        }

        void fromRawString(const QString &data)
        {
                             //"04263214911","","13/10/23,16:06:06-18"\r\nPrueba e,0,0
            //Format => +CMT: "+15196168742","","13/10/21,09:29:28-18"\n1 2 3 testing messaging,0,0

            QRegExp regExp("^\"([^\"]*)\",\"([^\"]*)\",\"([^\"]*)\"(.+),(\\d),(\\d)$");
            if (regExp.indexIn(data) > -1 && regExp.captureCount() >= 4)
            {
                _destinationNumber = regExp.cap(1).trimmed();
                _date = QDate::fromString(regExp.cap(3).trimmed(), QString("DD/MM/YY,HH:MM:SS"));
                _message = regExp.cap(4).trimmed();
            }
            else
            {
                _valid = false;
            }
        }

        QString from() const
        {
            return _destinationNumber;
        }

        QDate date() const
        {
            return _date;
        }

        QString body() const
        {
            return _message;
        }

    private:

        QString _destinationNumber;
        QDate _date;
        QString _message;
    };
}

#endif // INCOMINGSMSRECORD_HPP
