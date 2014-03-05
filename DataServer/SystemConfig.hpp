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
    typedef QList<WatcherInfo> WatcherInfoList;
    typedef QMap<QString, QString> ProxyInfo;

    class SystemConfig : public QObject
    {
        Q_OBJECT
    public:
        static bool initialize(const QString &configFile);
        static SystemConfig *instance();
        static void destroyInstance();

        DeviceInfoList devicesInfo() const;
        WatcherInfoList watchersInfo() const;

        ProxyInfo proxyInfo() const;

        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

    private:
        SystemConfig(const QString &configFile);
        ~SystemConfig();

    private:
        static SystemConfig *_instance;
        QSettings *_settings;
    };
}
#endif // SYSTEMCONFIG_HPP
