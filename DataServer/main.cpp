#include <QCoreApplication>

#include <SystemEngine.hpp>
#include <SystemConfig.hpp>

#include <iostream>
#include <string>
#include <QDateTime>

using namespace Gateway;

void MsgOuput(QtMsgType type, const char *msg)
{
    std::string dateTime = QDateTime::currentDateTime()
            .toString()
            .toStdString();

    switch (type)
    {
        case QtDebugMsg:
            std::cout << "(II) " << dateTime << " " << msg << std::endl;
            break;
        case QtWarningMsg:
            std::cerr << "(WW) " << dateTime << " " << msg << std::endl;
            break;
        case QtCriticalMsg:
            std::cerr << "(EE) " << dateTime << " " << msg << std::endl;
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(MsgOuput);
    QCoreApplication app(argc, argv);

    //TODO hola mundo

    QString configFile("");
    foreach (QString option, app.arguments())
    {
        if (option.startsWith("--config"))
            configFile = option.mid(strlen("--config") + 1);
    }

    SystemConfig::initialize(configFile);
    SystemEngine *engine = new SystemEngine;
    Q_UNUSED(engine);

    qDebug("Running ...");
    return app.exec();
}
