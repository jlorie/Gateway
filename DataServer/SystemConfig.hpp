#ifndef SYSTEMCONFIG_HPP
#define SYSTEMCONFIG_HPP

#include <include/DataStructures/DeviceInfo.hpp>
#include <include/DataStructures/WatcherInfo.hpp>

#include <QObject>
#include <QSettings>
#include <QStringList>

namespace Gateway
{
    typedef QList<DeviceInfo> DeviceInfoList;

    class SystemConfig : public QObject
    {
        Q_OBJECT
    public:
        static bool initialize();
        static SystemConfig *instance();
        static void destroyInstance();

        DeviceInfoList devicesInfo() const;
        WatcherInfo *watcherInfo() const;

        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

        void loadSettings();
        void saveSettings();

    private:
        SystemConfig();
        ~SystemConfig();

    private:
        static SystemConfig *_instance;

        DeviceInfoList _devicesInfo;
        WatcherInfo *_watcherInfo;
        QSettings *_settings;
    };
}
#endif // SYSTEMCONFIG_HPP
