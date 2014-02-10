#ifndef SYSTEMCONFIG_HPP
#define SYSTEMCONFIG_HPP

#include <include/DataStructures/DeviceInfo.hpp>
#include <include/DataStructures/WatcherInfo.hpp>
#include <include/DataStructures/MainInfo.hpp>

#include <QObject>
#include <QSettings>
#include <QStringList>

namespace Gateway
{
    typedef QList<DeviceInfo> DeviceInfoList;
    typedef QList<WatcherInfo *> WatcherInfoList;

    class SystemConfig : public QObject
    {
        Q_OBJECT
    public:
        static bool initialize(const QString &configFile);
        static SystemConfig *instance();
        static void destroyInstance();

        DeviceInfoList devicesInfo() const;
        WatcherInfoList watchersInfo() const;

        MainInfo *mainInfo() const;

        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

    private:
        SystemConfig(const QString &configFile);
        ~SystemConfig();

        void loadSettings();

    private:
        static SystemConfig *_instance;

        DeviceInfoList _devicesInfo;
        WatcherInfoList _watchersInfo;

        MainInfo *_mainInfo;

        QSettings *_settings;
    };
}
#endif // SYSTEMCONFIG_HPP
