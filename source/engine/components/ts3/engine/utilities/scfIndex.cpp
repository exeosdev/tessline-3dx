
#include "scfIndex.h"
#include <ts3/engine/exception.h>
#include <ts3/stdext/pathNameIterator.h>

namespace ts3
{

    SCFIndex::SCFIndex() = default;

    SCFIndex::~SCFIndex() = default;

    uint64 SCFIndex::readResourceData( void * pTarget, uint64 pSize, uint64 pOffset ) const
    {
        if( !_rdReadCallback )
        {
            return 0;
        }
        return _rdReadCallback( pTarget, pSize, pOffset );
    }

    SCFVirtualFolder & SCFIndex::rootFolder() const
    {
        return *_rootFolder;
    }

    SCFVirtualFolder & SCFIndex::initRootFolder( SCFVirtualFolderInfo pFolderInfo )
    {
        _rootFolder = std::make_unique<SCFVirtualFolder>( nullptr, std::move( pFolderInfo ) );
        return *_rootFolder;
    }

    void SCFIndex::addEntry( SCFEntry & pEntry )
    {
        if( !pEntry.mInfo->uid.empty() )
        {
            // Add UID to the global UID map?
        }
    }

    void SCFIndex::resetIndex()
    {
    }

}
