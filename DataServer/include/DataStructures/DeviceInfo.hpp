#ifndef DEVICEINFO_HPP
#define DEVICEINFO_HPP

#include <QMap>
#include <QString>
#include <qstringlist.h>

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

   QStringList ports ()
   {
       return _ports;
   }

   void addPort (const QString & item)
   {
       _ports.append(item);
   }

private:
   QStringList _ports;
};

#endif // DEVICEINFO_HPP
