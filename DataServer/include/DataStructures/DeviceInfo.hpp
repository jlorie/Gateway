#ifndef DEVICEINFO_HPP
#define DEVICEINFO_HPP

#include <QMap>
#include <QString>
#include <QStringList>

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

    void addPort(const QString &newPort)
    {
        _ports.append(newPort);
    }

    QStringList ports() const
    {
        return _ports;
    }

private:
    QStringList _ports;
};

#endif // DEVICEINFO_HPP
