#include <SystemEngine.hpp>

#include <QApplication>
#include <QNetworkProxy>
#include <iostream>

#include <QDebug>

using namespace Gateway;

void setProxy()
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("10.121.6.12");
    proxy.setPort(8080);

    QNetworkProxy::setApplicationProxy(proxy);
}

void MsgOuput(QtMsgType type, const QMessageLogContext &context, const QString& qmsg)
{
    Q_UNUSED(context);
    const char* msg = qPrintable(qmsg);

    switch (type)
    {
        case QtDebugMsg:
            std::cout << "(II) " << msg << std::endl;
            break;
        case QtWarningMsg:
            std::cerr << "(WW) " << msg << std::endl;
//            std::cerr << "(WW)    >> " << context.file << " line " << context.line << " on " << context.function << std::endl;
            break;
        case QtCriticalMsg:
            std::cerr << "(EE) " << msg << std::endl;
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(MsgOuput);
    QApplication a(argc, argv);

//    setProxy();
    SystemEngine *engine = new SystemEngine;
    Q_UNUSED(engine);

    qDebug("Running ...");
    return a.exec();
}
