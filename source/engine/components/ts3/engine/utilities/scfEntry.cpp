
#include "scfEntry.h"
#include <ts3/engine/exception.h>
#include <ts3/stdext/pathNameIterator.h>

namespace ts3
{

	SCFEntry::SCFEntry( SCFVirtualFolder * pParentFolder, const SCFEntryInfo * pInfo )
	: mInfo( pInfo )
	, mParentFolder( pParentFolder )
	{}

	SCFEntry::~SCFEntry() = default;

	SCFVirtualFolder & SCFEntry::asVirtualFolder()
	{
	    return dynamic_cast<SCFVirtualFolder &>( *this );
	}

	const SCFVirtualFolder & SCFEntry::asVirtualFolder() const
	{
	    return dynamic_cast<const SCFVirtualFolder &>( *this );
	}

	SCFVirtualFolder * SCFEntry::asVirtualFolderPtr() noexcept
	{
	    return dynamic_cast<SCFVirtualFolder *>( this );
	}

	const SCFVirtualFolder * SCFEntry::asVirtualFolderPtr() const noexcept
	{
	    return dynamic_cast<const SCFVirtualFolder *>( this );
	}

	SCFResource & SCFEntry::asResource()
	{
	    return dynamic_cast<SCFResource &>( *this );
	}

	const SCFResource & SCFEntry::asResource() const
	{
	    return dynamic_cast<const SCFResource &>( *this );
	}

	SCFResource * SCFEntry::asResourcePtr() noexcept
	{
	    return dynamic_cast<SCFResource *>( this );
	}

	const SCFResource * SCFEntry::asResourcePtr() const noexcept
	{
	    return dynamic_cast<const SCFResource *>( this );
	}


	SCFResource::SCFResource( SCFVirtualFolder & pParentFolder, SCFResourceInfo pInfo )
	: SCFEntry( &pParentFolder, &mResourceInfo )
	, mResourceInfo( std::move( pInfo ) )
	{}


	SCFVirtualFolder::SCFVirtualFolder( SCFVirtualFolder * pParentFolder, SCFVirtualFolderInfo pInfo )
	: SCFEntry( pParentFolder, &mFolderInfo )
	, mFolderInfo( std::move( pInfo ) )
	{}

	SCFEntry * SCFVirtualFolder::operator[]( const std::string & pEntryPath ) const noexcept
	{
		return findEntry( pEntryPath, ESCFFindMode::Default );
	}

	SCFEntry & SCFVirtualFolder::at( const std::string & pEntryPath ) const
	{
		return getEntry( pEntryPath, ESCFFindMode::Default );
	}

	SCFEntry * SCFVirtualFolder::findEntry( const std::string & pEntryPath, ESCFFindMode pFindMode ) const noexcept
	{
		auto * entryPtr = findEntryInternal( pEntryPath, pFindMode );
		if( !entryPtr )
		{
			return nullptr;
		}
		return entryPtr;
	}

	SCFEntry & SCFVirtualFolder::getEntry( const std::string & pEntryPath, ESCFFindMode pFindMode ) const
	{
		auto * entryPtr = findEntryInternal( pEntryPath, pFindMode );
		if( !entryPtr )
		{
			ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, pEntryPath + " not found in " + mInfo->name );
		}
		return *entryPtr;
	}

	SCFVirtualFolder * SCFVirtualFolder::findVirtualFolder( const std::string & pFolderPath, ESCFFindMode pFindMode ) const noexcept
	{
		auto * entryPtr = findEntryInternal( pFolderPath, pFindMode );
		if( !entryPtr || !entryPtr->isVirtualFolder() )
		{
			return nullptr;
		}
		return entryPtr->asVirtualFolderPtr();
	}

	SCFVirtualFolder & SCFVirtualFolder::getVirtualFolder( const std::string & pFolderPath, ESCFFindMode pFindMode ) const
	{
		auto * entryPtr = findEntryInternal( pFolderPath, pFindMode );
		if( !entryPtr )
		{
			ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, pFolderPath + " not found in " + mInfo->name );
		}
		if( !entryPtr->isVirtualFolder() )
		{
			ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, pFolderPath + " in " + mInfo->name + " is not a folder" );
		}
		return entryPtr->asVirtualFolder();
	}

	SCFResource * SCFVirtualFolder::findResource( const std::string & pResourcePath, ESCFFindMode pFindMode ) const noexcept
	{
		auto * entryPtr = findEntryInternal( pResourcePath, pFindMode );
		if( !entryPtr || !entryPtr->isResource() )
		{
			return nullptr;
		}
		return entryPtr->asResourcePtr();
	}

	SCFResource & SCFVirtualFolder::getResource( const std::string & pResourcePath, ESCFFindMode pFindMode ) const
	{
		auto * entryPtr = findEntryInternal( pResourcePath, pFindMode );
		if( !entryPtr )
		{
			ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, pResourcePath + " not found in " + mInfo->name );
		}
		if( !entryPtr->isResource() )
		{
			ts3ThrowDesc( E_EXCEPTION_CODE_ENGINE_SM_MAIN_SCF_ERROR, pResourcePath + " in " + mInfo->name + " is not a resource" );
		}
		return entryPtr->asResource();
	}

	bool SCFVirtualFolder::enumerateEntries( EntryList & pList, bool pClearList ) const
	{
		if( pClearList )
		{
			pList.clear();
		}

		const auto inputSize = pList.size();

		for( const auto & resourcePtr : _resourceList )
		{
			pList.push_back( resourcePtr.get() );
		}

		for( const auto & subFolderPtr : _subFolderList )
		{
			pList.push_back( subFolderPtr.get() );

			subFolderPtr->enumerateEntries( pList );
		}

		return pList.size() > inputSize;
	}

	bool SCFVirtualFolder::enumerateEntries( EntryList & pList, const EntryPredicate & pPredicate, bool pClearList ) const
	{
		if( pClearList )
		{
			pList.clear();
		}

		const auto inputSize = pList.size();

		for( const auto & resourcePtr : _resourceList )
		{
			if( pPredicate( *resourcePtr ) )
			{
				pList.push_back( resourcePtr.get() );
			}
		}

		for( const auto & subFolderPtr : _subFolderList )
		{
			if( pPredicate( *subFolderPtr ) )
			{
				pList.push_back( subFolderPtr.get() );
			}
			subFolderPtr->enumerateEntries( pList, pPredicate );
		}

		return pList.size() > inputSize;
	}

	SCFVirtualFolder::EntryList SCFVirtualFolder::enumerateEntries() const
	{
		EntryList result{};
		enumerateEntries( result );
		return result;
	}

	SCFVirtualFolder::EntryList SCFVirtualFolder::enumerateEntries( const EntryPredicate & pPredicate ) const
	{
		EntryList result{};
		enumerateEntries( result, pPredicate );
		return result;
	}

	SCFResource & SCFVirtualFolder::addResource( SCFResourceInfo pResourceInfo )
	{
		auto newResourcePtr = std::make_unique<SCFResource>( *this, std::move( pResourceInfo ) );

		auto & newResource = *newResourcePtr;

		_entryNameMap.insert( { newResourcePtr->mResourceInfo.name, newResourcePtr.get() } );
		_resourceList.push_back( std::move( newResourcePtr ) );

		return newResource;
	}

	SCFVirtualFolder & SCFVirtualFolder::addSubFolder( SCFVirtualFolderInfo pFolderInfo )
	{
		auto newFolderPtr = std::make_unique<SCFVirtualFolder>( this, std::move( pFolderInfo ) );

		auto & newFolder = *newFolderPtr;

		_entryNameMap.insert( { newFolderPtr->mFolderInfo.name, newFolderPtr.get() } );
		_subFolderList.push_back( std::move( newFolderPtr ) );

		return newFolder;
	}

	SCFEntry * SCFVirtualFolder::findEntryInternal( const std::string & pEntryPath, ESCFFindMode pFindMode ) const noexcept
	{
		if( pFindMode == ESCFFindMode::Direct )
		{
			return findEntryInternalDirect( pEntryPath );
		}
		else
		{
			return findEntryInternalRecursive( pEntryPath );
		}
	}

	SCFEntry * SCFVirtualFolder::findEntryInternalDirect( const std::string & pEntryName ) const noexcept
	{
		auto entryPtrIter = _entryNameMap.find( pEntryName );
		if( entryPtrIter == _entryNameMap.end() )
		{
			return nullptr;
		}
		return entryPtrIter->second;
	}

	SCFEntry * SCFVirtualFolder::findEntryInternalRecursive( const std::string & pEntryPath ) const noexcept
	{
		// Gets a pointer to an item (a sub-folder or a resource) within this folder.
		// Example: "/resources/textures" will return a pointer to the "textures" folder,
		// while "/resources/textures/basic_terrain0.dds" will fetch the texture resource.
		// If specified sub-folder/resource does not exist, a nullptr is returned.

		// Current entry pointer, initially this.
		SCFEntry * currentEntryPtr = const_cast<SCFVirtualFolder *>( this );

		auto pathNameIterator = PathNameIterator( pEntryPath );

		// This could be cleaner in the recursive version, but we hit couple use cases with
		// deep SCF structures, where iteration-based approach turned out to be much better.
		// This version also uses a local string updated in-place instead of passing a new
		// string upon each invocation in the original recursive approach.
		while( currentEntryPtr && !pathNameIterator.empty() )
		{
			if( !currentEntryPtr->isVirtualFolder() )
			{
				// pathNameIterator still has some content (which means a sub-folder/resource
				// still needs to be found), but we hit an item which is not a folder - thus,
				// cannot have any nested items.
				return nullptr;
			}

			// This should never fail, as we checked that this item is a folder.
			auto & itemAsVirtualFolder = currentEntryPtr->asVirtualFolder();

			const auto & nextEntryName = pathNameIterator.name();

			if( nextEntryName == "." )
			{
				continue;
			}
			else if( nextEntryName == ".." )
			{
				currentEntryPtr = mParentFolder;
			}
			else
			{
				// Get the next item from the internal map.
				auto nextEntryPtrIter = itemAsVirtualFolder._entryNameMap.find( nextEntryName );

				if( nextEntryPtrIter == itemAsVirtualFolder._entryNameMap.end() )
				{
					// Not found, return nullptr.
					return nullptr;
				}

				currentEntryPtr = nextEntryPtrIter->second;
			}
		}

		return currentEntryPtr;
	}

}
