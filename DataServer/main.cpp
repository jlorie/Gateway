#include "ui/mainwindow.h"

#include <DeviceManager.hpp>
#include <AccountManager.hpp>
#include <storage/LocalStorage.hpp>
#include <common/CommonErrors.hpp>

#include <QApplication>
#include <QNetworkProxy>
#include <QString>

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    setProxy();

    StorageConfig config;
    config["local_storage_library"] = "/home/lorie/workspace/My Projects/_qt-builds/build-gateway-Desktop-Debug/LiteStorage/libLiteStorage.so";
    config["local_storage_db_path"] = "/home/lorie/workspace/My Projects/_qt-builds/gateway.db";

    qDebug("Initializing LocalStorage");
    {
        LocalStorage::initialize(config);
    //    LocalStorage *localStorage = LocalStorage::instance();
    }

    qDebug("Initializing AccountManager");
    {
         AccountManager::initialize();
    }

    qDebug(">> Initializing DeviceManager");
    {
        DeviceManager::initialize();
//        DeviceManager *devManager = DeviceManager::instance();
    }

    MainWindow window;
    window.show();
    qDebug("End");
    return a.exec();
}




//    //listing SMS
//    {
//        QMap<QString, QString> filter;
//        filter.insert(QString("DateSent"), QString("2013-10-15"));

//        test.listSMS(filter);
//    }
