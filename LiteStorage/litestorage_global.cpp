#include "litestorage_global.hpp"
#include "LiteStorage.hpp"

LITESTORAGESHARED_EXPORT IStorage* initStorageLibrary()
{
    return new LiteStorage();
}

LITESTORAGESHARED_EXPORT void closeStorageLibrary(IStorage *storage)
{
    delete storage;
}
