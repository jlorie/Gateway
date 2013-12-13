#include <DeviceManager.hpp>
#include <Storage.hpp>

#include <common/Rule.hpp>

#include <QApplication>
#include <QNetworkProxy>
#include <QString>
#include <iostream>

#include <QStringList>
#include <QRegExp>

using namespace Gateway;

void setProxy()
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("10.121.6.12");
    proxy.setPort(8080);

    QNetworkProxy::setApplicationProxy(proxy);
}

#include <QFile>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#define qInstallMessageHandler qInstallMsgHandler
void MsgOuput(QtMsgType type, const char *msg)
{
#else
void MsgOuput(QtMsgType type, const QMessageLogContext &, const QString& qmsg)
{
    const char* msg = qPrintable(qmsg);
#endif

    switch (type)
    {
        case QtDebugMsg:
            std::cout << "(II) " << msg << std::endl;
            break;
        case QtWarningMsg:
            std::cerr << "(WW) " << msg << std::endl;
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
    setProxy();

    StorageConfig config;
    config["storage_library"] = "/home/lorie/workspace/My Projects/_qt-builds/build-gateway-Desktop-Debug/libs/libLiteStorage.so";
    config["local_storage_db_path"] = "/home/lorie/workspace/My Projects/_qt-builds/gateway.db";

    qDebug("Initializing Storage");
    {
        Storage::initialize(config);
    }

    qDebug(">> Initializing DeviceManager");
    {
        DeviceManager::initialize();
//        DeviceManager *devManager = DeviceManager::instance();
    }

    qDebug("Running ...");
    return a.exec();
}
