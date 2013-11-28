#ifndef LOCALSTORAGE_HPP
#define LOCALSTORAGE_HPP

#include "Storage.hpp"

#include <QLibrary>

namespace Gateway
{
    class LocalStorage : public Storage
    {
        public:
            static LocalStorage * instance();
            static void initialize(const StorageConfig &config);
            static void destroyInstance();


        private:
            LocalStorage(const StorageConfig &config);

        private:
            static LocalStorage * _instance;

            QLibrary _library;
    };
}
#endif // LOCALSTORAGE_HPP
