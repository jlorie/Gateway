#include "Storage.hpp"

#include <QFile>
#include <QPluginLoader>

namespace Gateway
{
    Storage * Storage::_instance = 0;

    Storage *Storage::instance()
    {
        return _instance;
    }

    void Storage::initialize(const StorageConfig &config)
    {
        if (!_instance)
            _instance = new Storage(config);
    }


    Storage::Storage(const StorageConfig &config)
        :_storage(0)
    {
        QString fileName = config.value(QString("storage_library"), QString(""));

        // Loading storage
        if (QFile::exists(fileName))
        {
            QPluginLoader loader(fileName);
            QObject *library = loader.instance();
            if (library)
            {
                qDebug("Plugin found: %s", qPrintable(fileName));

                _storage = qobject_cast<IStorage *>(library);
                if (_storage)
                {
                    if (_storage->configure(config))
                        qDebug(">> Storage ready!!!");
                    else
                        qWarning("Error configuring storage");
                }
                else
                {
                    qWarning("Invalid storage library: %s", qPrintable(fileName));
                    return;
                }
            }
            else
            {
                qWarning("Could not open library: %s", qPrintable(fileName));
            }
        }
        else
        {
            qWarning("Could not find library %s", qPrintable(fileName));
        }

    }

    IRule *Storage::ruleFor(const IRule *rule) const
    {
        return _storage->ruleFor(rule);
    }

}
