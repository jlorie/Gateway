#ifndef DEVICEINFO_HPP
#define DEVICEINFO_HPP

#include <QMap>
#include <QString>

class DeviceInfo: public QMap<QString, QString>
{
public:
    QString toString()
    {
        QString result;

        foreach (QString key, keys())
        {
            result +=  key + ": " + value(key) + "\n";
        }

        return result;
    }

    bool isEnabled()
    {
        return (value("device_enabled").toInt() == 1);
    }
};

#endif // DEVICEINFO_HPP
