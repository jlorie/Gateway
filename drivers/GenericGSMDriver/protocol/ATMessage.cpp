#include "ATMessage.hpp"
#include <protocol/records/IncomingSMSRecord.hpp>
#include <Utils.hpp>

#include <QStringList>
#include <QDebug>

const char CR = 0x0D;
const char LF = 0x0A;
const char BS = 0x08;
const char CZ = 0x1A;

ATMessage::ATMessage()
{
}

QString ATMessage::assemble(ATCommand command, const CommandArgs &args)
{
    QString result;

    switch (command)
    {
        case cmAT:
        {
            result = QString("AT").append(CR);
            break;
        }
        case cmCMGF:
        {
            result = setTextModeCommand(args["mode"]);
            break;
        }
        case cmCNMI:
        {
            //FIXME arreglar y ponerle los nombres a los parametros
            result = setMessageIndicationCommand(args["mode"]);
            break;
        }
        case cmCMGS:
        {
            break;
        }
        default:
            break;
    }

    return result;
}

AbstractRecord *ATMessage::disassemble(const QString &frame)
{
    AbstractRecord *result = 0;

    int commandEndPos(frame.indexOf(":"));
    if (commandEndPos > -1)
    {
        QString command (Util::trim(frame.mid(0, commandEndPos)));
        QString data (Util::trim(frame.mid(commandEndPos + 1)));

//qDebug() << "command " << command;
//qDebug() << "data " << data;

        if (command.contains("+CMT"))
        {
            IncomingSMSRecord *newRecord = new IncomingSMSRecord;
            newRecord->fromRawString(data);

            if (newRecord->isValid())
                result = newRecord;
        }
    }

    return result;
}

QString ATMessage::setTextModeCommand(const QString &mode)
{
    return QString("AT+CMGF=%1").arg(mode).append(CR);
}

QString ATMessage::setMessageIndicationCommand(const QString &mode)
{
    return QString("AT+CNMI=%1").arg(mode).append(CR);
}

QString ATMessage::sendMessage (const QString &number)
{
    QString a (QString("AT+CMGS=%1").arg(number).append(CR));
    qDebug ("cmd: %s", qPrintable(a));
    return a;
}
