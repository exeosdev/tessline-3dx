
#include "scfEntry.h"
#include "scfIndex.h"
#include <ts3/engine/exception.h>
#include <ts3/stdext/byteArray.h>
#include <ts3/stdext/memoryBuffer.h>
#include <ts3/stdext/pathNameIterator.h>

namespace ts3
{

	SCFEntry::SCFEntry( SCFIndex & pIndex, const SCFEntryInfo * pInfo )
	: mIndex( &pIndex )
	, mParentFolder( nullptr )
	, mInfo( pInfo )
	{}

	SCFEntry::SCFEntry( SCFVirtualFolder & pParentFolder, const SCFEntryInfo * pInfo )
	: mIndex( pParentFolder.mIndex )
	, mParentFolder( &pParentFolder )
	, mInfo( pInfo )
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
	: SCFEntry( pParentFolder, &mResourceInfo )
	, mResourceInfo( std::move( pInfo ) )
	{}

	uint64 SCFResource::readData( void * pTarget, uint64 pCapacity ) const
	{
		if( !pTarget || ( pCapacity < mResourceInfo.dataSize ) )
		{
			return 0;
		}
		return mIndex->readResourceData( pTarget, mResourceInfo.dataSize, mResourceInfo.dataOffset );
	}

	uint64 SCFResource::readData( DynamicByteArray & pTarget ) const
	{
		if( pTarget.size() < mResourceInfo.dataSize )
		{
			pTarget.resize( mResourceInfo.dataSize );
		}
		return readData( pTarget.data(), pTarget.size() );
	}

	uint64 SCFResource::readData( DynamicMemoryBuffer & pTarget ) const
	{
		if( pTarget.size() < mResourceInfo.dataSize )
		{
			pTarget.resize( mResourceInfo.dataSize );
		}
		return readData( pTarget.data(), pTarget.size() );
	}

	uint64 SCFResource::readData( std::string & pTarget ) const
	{
		if( pTarget.length() < mResourceInfo.dataSize )
		{
			pTarget.resize( mResourceInfo.dataSize );
		}
		return readData( pTarget.data(), pTarget.length() );
	}

	uint64 SCFResource::readData( std::vector<byte> & pTarget ) const
	{
		if( pTarget.size() < mResourceInfo.dataSize )
		{
			pTarget.resize( mResourceInfo.dataSize );
		}
		return readData( pTarget.data(), pTarget.size() );
	}

	uint64 SCFResource::readSubData( void * pTarget, uint64 pCapacity, uint64 pReadSize, uint64 pResOffset ) const
	{
		if( !pTarget || ( pCapacity == 0 ) || ( pReadSize == 0 ) || ( pResOffset >= mResourceInfo.dataSize ) )
		{
			return 0;
		}

		const auto maxDataSize = mResourceInfo.dataSize - pResOffset;
		const auto maxReadSize = getMinOf( pCapacity, maxDataSize );
		const auto readSize = getMinOf( pReadSize, maxReadSize );

		return mIndex->readResourceData( pTarget, readSize, mResourceInfo.dataOffset + pResOffset );
	}

	uint64 SCFResource::readSubData( const ReadWriteMemoryView & pTarget, uint64 pReadSize, uint64 pResOffset ) const
	{
		return readSubData( pTarget.data(), pTarget.size(), pReadSize, pResOffset );
	}

	uint64 SCFResource::readSubData( std::vector<byte> & pTarget, uint64 pReadSize, uint64 pResOffset ) const
	{
		return readSubData( pTarget.data(), pTarget.size(), pReadSize, pResOffset );
	}


	SCFVirtualFolder::SCFVirtualFolder( SCFIndex & pIndex, SCFVirtualFolderInfo pInfo )
	: SCFEntry( pIndex, &mFolderInfo )
	, mFolderInfo( std::move( pInfo ) )
	{}

	SCFVirtualFolder::SCFVirtualFolder( SCFVirtualFolder & pParentFolder, SCFVirtualFolderInfo pInfo )
	: SCFEntry( pParentFolder, &mFolderInfo )
	, mFolderInfo( std::move( pInfo ) )
	{}

	SCFEntry * SCFVirtualFolder::operator[]( const std::string & pEntryPath ) const noexcept
	{
		return findEntry( pEntryPath, ESCFFindMode::Direct );
	}

	SCFEntry & SCFVirtualFolder::at( const std::string & pEntryPath ) const
	{
		return getEntry( pEntryPath, ESCFFindMode::Direct );
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
			ts3ThrowDesc( E_EXC_ESM_MAIN_SCF_ERROR, pEntryPath + " not found in " + mInfo->name );
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
			ts3ThrowDesc( E_EXC_ESM_MAIN_SCF_ERROR, pFolderPath + " not found in " + mInfo->name );
		}
		if( !entryPtr->isVirtualFolder() )
		{
			ts3ThrowDesc( E_EXC_ESM_MAIN_SCF_ERROR, pFolderPath + " in " + mInfo->name + " is not a folder" );
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
			ts3ThrowDesc( E_EXC_ESM_MAIN_SCF_ERROR, pResourcePath + " not found in " + mInfo->name );
		}
		if( !entryPtr->isResource() )
		{
			ts3ThrowDesc( E_EXC_ESM_MAIN_SCF_ERROR, pResourcePath + " in " + mInfo->name + " is not a resource" );
		}
		return entryPtr->asResource();
	}

	bool SCFVirtualFolder::enumerateEntries( SCFEntryList & pList, ESCFFindMode pFindMode, bool pClearList ) const
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

		if( pFindMode == ESCFFindMode::Recursive )
		{
			for( const auto & subFolderPtr : _subFolderList )
			{
				pList.push_back( subFolderPtr.get() );

				subFolderPtr->enumerateEntries( pList, pFindMode );
			}
		}

		return pList.size() > inputSize;
	}

	bool SCFVirtualFolder::enumerateEntries( SCFEntryList & pList, ESCFFindMode pFindMode, const SCFEntryPredicate & pPredicate, bool pClearList ) const
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

		if( pFindMode == ESCFFindMode::Recursive )
		{
			for( const auto & subFolderPtr : _subFolderList )
			{
				if( pPredicate( *subFolderPtr ) )
				{
					pList.push_back( subFolderPtr.get() );
				}
				subFolderPtr->enumerateEntries( pList, pFindMode, pPredicate );
			}
		}

		return pList.size() > inputSize;
	}

	SCFEntryList SCFVirtualFolder::enumerateEntries( ESCFFindMode pFindMode ) const
	{
		SCFEntryList result{};
		enumerateEntries( result, pFindMode );
		return result;
	}

	SCFEntryList SCFVirtualFolder::enumerateEntries( ESCFFindMode pFindMode, const SCFEntryPredicate & pPredicate ) const
	{
		SCFEntryList result{};
		enumerateEntries( result, pFindMode, pPredicate );
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
		auto newFolderPtr = std::make_unique<SCFVirtualFolder>( *this, std::move( pFolderInfo ) );

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

			++pathNameIterator;
		}

		return currentEntryPtr;
	}

} // namespace ts3
