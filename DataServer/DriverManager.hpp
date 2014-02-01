#ifndef DRIVERMANAGER_HPP
#define DRIVERMANAGER_HPP

#include <include/DriverInterface.hpp>

namespace Gateway
{
    typedef QList<DriverInterface *> DriverList;

    class DriverManager
    {
    public:
        static void initialize();
        static DriverManager *instance();
        static void destroyInstance();

        DriverList availableDrivers();
        DriverInterface *driverFor(const QString &driverName);

    private:
        DriverManager();
        ~DriverManager();

    private:
        DriverList _driverLibraries;
        static DriverManager *_instance;
    };
}
#endif // DRIVERMANAGER_HPP
