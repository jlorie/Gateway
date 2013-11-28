#include "LocalStorage.hpp"

#include <QFile>

namespace Gateway
{
    LocalStorage * LocalStorage::_instance = 0;

    LocalStorage *LocalStorage::instance()
    {
        return _instance;
    }

    void LocalStorage::initialize(const StorageConfig &config)
    {
        if (!_instance)
            _instance = new LocalStorage(config);
    }


    LocalStorage::LocalStorage(const StorageConfig &config)
        :Storage()
    {
        QString driverLibrary = config.value(QString("local_storage_library"), QString(""));

        // Loading local storage
        if (QFile::exists(driverLibrary))
        {
            _library.setFileName(driverLibrary);
            if (_library.load())
            {
                typedef IStorage* (*InitLibraryFunc)();
                InitLibraryFunc func = (InitLibraryFunc)_library.resolve ("initStorageLibrary");

                if (func)
                {
                    _storage = func();
                    if (_storage->configure(config))
                        qDebug("Storage ready");
                    else
                        qWarning("Error configuring local storage");
                }
                else
                {
                    qWarning("Could not resolve initStorageLibrary");
                }
            }
            else
            {
                qWarning("Could not load library");
            }
        }
        else
        {
            qWarning("Could not find library %s ", qPrintable(driverLibrary));
        }
    }
}
