#ifndef LITESTORAGE_GLOBAL_HPP
#define LITESTORAGE_GLOBAL_HPP

#include <QtCore/qglobal.h>
#include <IStorage.hpp>

#if defined(LITESTORAGE_LIBRARY)
#  define LITESTORAGESHARED_EXPORT Q_DECL_EXPORT
#else
#  define LITESTORAGESHARED_EXPORT Q_DECL_IMPORT
#endif

using namespace Gateway;

extern "C"
{
    LITESTORAGESHARED_EXPORT IStorage* initStorageLibrary();

    LITESTORAGESHARED_EXPORT void closeStorageLibrary(IStorage *storage);
}

#endif // LITESTORAGE_GLOBAL_HPP
