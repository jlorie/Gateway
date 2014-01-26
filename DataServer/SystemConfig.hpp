#ifndef SYSTEMCONFIG_HPP
#define SYSTEMCONFIG_HPP

#include <include/DeviceInfo.hpp>

#include <QObject>
#include <QSettings>

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

        DeviceInfoList devicesInfo();
        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

    private:
        SystemConfig();
        void loadSettings();

    private:
        static SystemConfig *_instance;

        DeviceInfoList _devicesInfo;
        QSettings *_settings;
    };
}
#endif // SYSTEMCONFIG_HPP
