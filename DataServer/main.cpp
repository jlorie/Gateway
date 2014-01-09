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
        DeviceManager *devManager = DeviceManager::instance();

        //Creating Serial Device
        {
            DeviceInfo info;
            {
                info.insert(QString("driver_name"), QString("GenericGSMDriver"));
                info.insert(QString("device_name"), QString("Samsung GT"));
                info.insert(QString("serial_port"), QString("ttyACM0"));
            }

            devManager->createDevice(info);
        }

        //Creating Web Device
        {
            DeviceInfo info;
            {
                info.insert(QString("driver_name"), QString("TelAPIWebDriver"));
                info.insert(QString("device_name"), QString("TelAPI"));
                info.insert(QString("device_user"), QString("AC5d198c28d93f4ae9912408c0bffc47c2"));
                info.insert(QString("device_password"), QString("2260cd6a2f4f4145a3f2a73d42b3d472"));
            }

            devManager->createDevice(info);
        }
    }

    qDebug("Running ...");
    return a.exec();
}
