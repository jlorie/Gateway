#include "MainWindow.hpp"
#include <QApplication>

#include "DeviceBrowser.hpp"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    Gateway::DriverManager::initialize();

    DeviceBrowser *devBrowser = new DeviceBrowser;
    {
        devBrowser->loadSettings();
        devBrowser->sync();
        devBrowser->saveSettings();
    }

    qDebug("End!");
    return a.exec();
}
