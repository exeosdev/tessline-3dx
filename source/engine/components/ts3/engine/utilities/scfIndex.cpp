
#include "scfIndex.h"
#include <ts3/engine/exception.h>
#include <ts3/stdext/pathNameIterator.h>

namespace ts3
{

    SCFIndex::SCFIndex() = default;

    SCFIndex::~SCFIndex() = default;

    SCFEntry * SCFIndex::findEntryByPath( const std::string & pEntryPath ) const noexcept
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->findEntry( pEntryPath, ESCFFindMode::Recursive );
    }

    SCFEntry & SCFIndex::getEntryByPath( const std::string & pEntryPath ) const
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->getEntry( pEntryPath, ESCFFindMode::Recursive );
    }

    SCFEntry * SCFIndex::findEntryByUUID( const std::string & pUUID ) const noexcept
    {
        ts3DebugAssert( _rootFolder );
        auto entryIter = _entryByUIDMap.find( pUUID );
        if( entryIter == _entryByUIDMap.end() )
        {
            return nullptr;
        }
        return entryIter->second;
    }

    SCFEntry & SCFIndex::getEntryByUUID( const std::string & pUUID ) const
    {
        ts3DebugAssert( _rootFolder );
        auto entryIter = _entryByUIDMap.find( pUUID );
        if( entryIter == _entryByUIDMap.end() )
        {
            ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, "UUID " + pUUID + " not found in the index" );
        }
        return *( entryIter->second );
    }

    bool SCFIndex::enumerateEntries( SCFEntryList & pList, bool pClearList ) const
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->enumerateEntries( pList, ESCFFindMode::Recursive, pClearList );
    }

    bool SCFIndex::enumerateEntries( SCFEntryList & pList, const SCFEntryPredicate & pPredicate, bool pClearList ) const
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->enumerateEntries( pList, ESCFFindMode::Recursive, pPredicate, pClearList );
    }

    SCFEntryList SCFIndex::enumerateEntries() const
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->enumerateEntries( ESCFFindMode::Recursive );
    }

    SCFEntryList SCFIndex::enumerateEntries( const SCFEntryPredicate & pPredicate ) const
    {
        ts3DebugAssert( _rootFolder );
        return _rootFolder->enumerateEntries( ESCFFindMode::Recursive, pPredicate );
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
            _entryByUIDMap[pEntry.mInfo->uid] = &pEntry;
        }
    }

    void SCFIndex::setResourceDataReadCallback( ResourceDataReadCallback pCallback )
    {
        _rdReadCallback = std::move( pCallback );
    }

    uint64 SCFIndex::readResourceData( void * pTarget, uint64 pSize, uint64 pOffset ) const
    {
        if( !_rdReadCallback )
        {
            return 0;
        }
        return _rdReadCallback( pTarget, pSize, pOffset );
    }

}
