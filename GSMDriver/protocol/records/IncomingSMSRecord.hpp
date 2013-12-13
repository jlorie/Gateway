#ifndef INCOMINGSMSRECORD_HPP
#define INCOMINGSMSRECORD_HPP

#include "AbstractRecord.hpp"
#include <Utils.hpp>

#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QDate>

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
        //          +CMT: "+447466829521","2013/11/29,21:47:59" 53028742 1 2 3 probando

        QStringList values = data.split(QRegExp("[,\\n]"));

        _destinationNumber = Util::trim(values.at(0));
        _date = QDate::fromString(values.at(1) + values.at(2), QString("DD/MM/YY,HH:MM:SS"));
        _message = Util::trim(values.at(4));


        //TODO tiene problemas con los mensajes con coma, arreglar
        _valid = true;
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

#endif // INCOMINGSMSRECORD_HPP