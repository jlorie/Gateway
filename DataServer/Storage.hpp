#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <include/IStorage.hpp>

#include <QObject>

namespace Gateway
{
    class Storage : public QObject
    {
        Q_OBJECT
    public:
        static Storage * instance();
        static void initialize(const StorageConfig &config);
        static void destroyInstance();

        virtual IRule *ruleFor(const IRule *rule) const;

    protected:
        IStorage *_storage;        
        static Storage * _instance;

    private:
        Storage(const StorageConfig &config);

    };
}
#endif // STORAGE_HPP
