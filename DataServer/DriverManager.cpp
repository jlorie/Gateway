#include "DriverManager.hpp"

#include <SystemConfig.hpp>

#include <QPluginLoader>
#include <QDir>

namespace Gateway
{
    DriverManager * DriverManager::_instance = 0;

    void DriverManager::initialize()
    {
        if (!_instance)
            _instance = new DriverManager();
    }

    DriverManager::~DriverManager()
    {
        foreach (DriverInterface *driver, _driverLibraries)
        {
            delete driver;
        }
    }

    DriverManager *DriverManager::instance()
    {
        return _instance;
    }

    DriverInterface *DriverManager::driverFor(const QString &driverName)
    {
        DriverInterface *result = 0;

        foreach (DriverInterface *driver, _driverLibraries)
        {
            if (driver->driverName() == driverName)
            {
                result = driver;
                break;
            }
        }

        return result;
    }

    DriverManager::DriverManager()
    {
        QString drivers_path = SystemConfig::instance()
                ->value("lib_path", "/usr/local/lib").toString();

        QDir driversDir(drivers_path);
        foreach (QString fileName, driversDir.entryList(QDir::Files))
        {
            QString canonicalPath = driversDir.canonicalPath() + QDir::separator() + fileName;

            QPluginLoader loader(canonicalPath);
            QObject *library = loader.instance();
            if (library)
            {
                DriverInterface *driver = qobject_cast<DriverInterface *>(library);
                if (driver)
                {
                    qDebug("Registering driver: %s", qPrintable(driver->driverName()));
                    qDebug(">>    %s", qPrintable(driver->description()));

                    _driverLibraries.append(driver);
                }
            }
            else
            {
                QString error = loader.errorString();
                error = error.mid(error.lastIndexOf(".so:") + 4);
                qWarning("Cannot load driverLibrary %s: %s", qPrintable(fileName), qPrintable(error.toLatin1()));
            }
        }
    }

}
