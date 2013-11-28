#include "ATMessage.hpp"
#include <devices/protocol/records/IncomingSMSRecord.hpp>

#include <QStringList>
#include <QDebug>

namespace Gateway
{
    const char CR = 0x0D;
    const char LF = 0x0A;
    const char BS = 0x08;

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
            QString command (frame.mid(0, commandEndPos).trimmed());
            QString data (frame.mid(commandEndPos + 1).trimmed());

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
}
