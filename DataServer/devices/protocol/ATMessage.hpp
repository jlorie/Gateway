#ifndef ATMESSAGE_HPP
#define ATMESSAGE_HPP

#include "records/AbstractRecord.hpp"
#include "ATCommand.hpp"

#include <QString>
namespace Gateway
{

    typedef QMap<QString, QString> CommandArgs;

    class ATMessage
    {
    public:
        ATMessage();

        QString assemble(ATCommand command, const CommandArgs &args);
        AbstractRecord *disassemble(const QString &frame);

        // AT Commands
        QString setTextModeCommand(const QString &mode);
        QString setMessageIndicationCommand(const QString &mode);
    };
}
#endif // ATMESSAGE_HPP
