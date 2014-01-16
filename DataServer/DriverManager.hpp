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
        ~DriverManager();

        static void destroy();

        static DriverManager *instance();

        DriverList availableDrivers();
        DriverInterface *driverFor(const QString &driverName);

    private:
        DriverManager(const QString &drivers_path);


    private:
        DriverList _driverLibraries;
        static DriverManager *_instance;
    };
}
#endif // DRIVERMANAGER_HPP
