#include <QCoreApplication>

#include <SystemEngine.hpp>
#include <SystemConfig.hpp>

#include <QStringList>

#include <QDebug>

using namespace Gateway;

//void MsgOuput(QtMsgType type, const QMessageLogContext &context, const QString& qmsg)
//{
//    Q_UNUSED(context);
//    const char* msg = qPrintable(qmsg);

//    switch (type)
//    {
//        case QtDebugMsg:
//            std::cout << "(II) " << msg << std::endl;
//            break;
//        case QtWarningMsg:
//            std::cerr << "(WW) " << msg << std::endl;
////            std::cerr << "(WW)    >> " << context.file << " line " << context.line << " on " << context.function << std::endl;
//            break;
//        case QtCriticalMsg:
//            std::cerr << "(EE) " << msg << std::endl;
//            break;
//        default:
//            break;
//    }
//}

int main(int argc, char *argv[])
{
//    qInstallMessageHandler(MsgOuput);
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
