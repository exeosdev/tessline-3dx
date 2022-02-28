
#include "scfIndexBuilder.h"
#include <ts3/engine/exception.h>
#include <ts3/stdext/pathNameIterator.h>
#include <ts3/stdext/sortedArray.h>
#include <unordered_map>
#include <unordered_set>

namespace ts3
{

	bool operator==( const SCFResourceTemplate & pLhs, const SCFResourceTemplate & pRhs )
	{
		return pLhs.name == pRhs.name;
	}
	
	bool operator==( const SCFResourceTemplate & pLhs, const std::string & pRhs )
	{
		return pLhs.name == pRhs;
	}
	
	bool operator<( const SCFResourceTemplate & pLhs, const SCFResourceTemplate & pRhs )
	{
		return pLhs.name < pRhs.name;
	}
	
	bool operator<( const SCFResourceTemplate & pLhs, const std::string & pRhs )
	{
		return pLhs.name < pRhs;
	}
	
	bool operator==( const SCFVirtualFolderTemplate & pLhs, const SCFVirtualFolderTemplate & pRhs )
	{
		return pLhs.name == pRhs.name;
	}
	
	bool operator==( const SCFVirtualFolderTemplate & pLhs, const std::string & pRhs )
	{
		return pLhs.name == pRhs;
	}
	
	bool operator<( const SCFVirtualFolderTemplate & pLhs, const SCFVirtualFolderTemplate & pRhs )
	{
		return pLhs.name < pRhs.name;
	}
	
	bool operator<( const SCFVirtualFolderTemplate & pLhs, const std::string & pRhs )
	{
		return pLhs.name < pRhs;
	}


	struct SCFIndexBuilder::PrivateWorkingData
	{
		SCFVirtualFolderTemplate rootFolder;
		std::unordered_set<std::string> globalPathSet;
		std::unordered_set<std::string> globalUIDSet;
	};


	const SCFVirtualFolderTemplate * SCFIndexBuilder::addFolder( const SCFVirtualFolderTemplate * pParentFolder,
																 std::string pFolderName,
																 std::string pUID )
	{
		if( !pParentFolder )
		{
			pParentFolder = &( _privateWorkingData->rootFolder );
		}

		return addFolder( pParentFolder->path, std::move( pFolderName ), std::move( pUID ) );
	}

	const SCFVirtualFolderTemplate * SCFIndexBuilder::addFolder( const std::string & pParentLocation,
																 std::string pFolderName,
																 std::string pUID )
	{
		auto * parentFolder = _findFolderInternal( pParentLocation, nullptr );
		if( !parentFolder )
		{
			return nullptr;
		}

		if( !checkNameAndUID( pFolderName, pUID ) )
		{
			return nullptr;
		}

		auto newFolderPath = parentFolder->path + "/" + pFolderName;

		auto existingPathIter = _privateWorkingData->globalPathSet.find( newFolderPath );
		if( existingPathIter != _privateWorkingData->globalPathSet.end() )
		{
			return nullptr;
		}

		SCFVirtualFolderTemplate virtualFolder;
		virtualFolder.name = std::move( pFolderName );
		virtualFolder.uid = std::move( pUID );
		virtualFolder.path = std::move( newFolderPath );

		// Add the folder to the sub-folder list of its parent.
		auto newFolderIter = parentFolder->subFolderList.insert( std::move( virtualFolder ) );

		// Insert the full path to global list for easier validation.
		_privateWorkingData->globalPathSet.insert( newFolderIter->path );

		if( !newFolderIter->uid.empty() )
		{
			// If UID was specified, add it to the global UID list as well.
			_privateWorkingData->globalUIDSet.insert( newFolderIter->uid );
		}

		return &( *newFolderIter );

	}

	const SCFResourceTemplate * SCFIndexBuilder::addResource( const SCFVirtualFolderTemplate * pParentFolder,
															  std::string pResourceName,
															  std::string pUID )
	{
		if( !pParentFolder )
		{
			pParentFolder = &( _privateWorkingData->rootFolder );
		}

		return addResource( pParentFolder->path, std::move( pResourceName ), std::move( pUID ) );
	}

	const SCFResourceTemplate * SCFIndexBuilder::addResource( const std::string & pParentLocation,
															  std::string pResourceName,
															  std::string pUID )
	{
		auto * parentFolder = _findFolderInternal( pParentLocation, nullptr );
		if( !parentFolder )
		{
			return nullptr;
		}

		if( !checkNameAndUID( pResourceName, pUID ) )
		{
			return nullptr;
		}

		auto newResourcePath = parentFolder->path + "/" + pResourceName;

		auto existingPathIter = _privateWorkingData->globalPathSet.find( newResourcePath );
		if( existingPathIter != _privateWorkingData->globalPathSet.end() )
		{
			return nullptr;
		}

		SCFResourceTemplate resource;
		resource.name = std::move( pResourceName );
		resource.uid = std::move( pUID );
		resource.path = std::move( newResourcePath );

		// Add the folder to the resource list of its parent.
		auto resourceIter = parentFolder->resourceList.insert( std::move( resource ) );

		// Insert the full path to global list for easier validation.
		_privateWorkingData->globalPathSet.insert( resourceIter->path );

		if( !resourceIter->uid.empty() )
		{
			// If UID was specified, add it to the global UID list as well.
			_privateWorkingData->globalUIDSet.insert( resourceIter->uid );
		}

		return &( *resourceIter );
	}

	bool SCFIndexBuilder::removeEntry(  const std::string & pLocation )
	{
	}

	bool SCFIndexBuilder::resetResourceInputData( const std::string & pResourceLocation )
	{
		auto * resourcePtr = _findResourceInternal( pResourceLocation );
		if( !resourcePtr )
		{
			return false;
		}

		resourcePtr->inputData = SCFResourceInputData{};

		return true;
	}

	bool SCFIndexBuilder::setResourceDataPlaceholderSize( const SCFResourceTemplate * pResource, uint64 pDataSize )
	{
		if( pResource && ( pDataSize > 0 ) )
		{
			SCFResourceInputData resInputData;
			resInputData.placeholderSize = pDataSize;
			resInputData.dataType = SCFResourceInputDataType::EmptyPlaceholder;

			return _setResourceInputData( pResource, std::move( resInputData ) );
		}

		return false;
	}

	bool SCFIndexBuilder::setResourceDataProvider( const SCFResourceTemplate * pResource,
												   SCFResourceInputData::DataProvider pDataProvider )
	{
		if( pResource && pDataProvider.readCallback && pDataProvider.sizeInfoCallback )
		{
			if( pDataProvider.sizeInfoCallback() > 0 )
			{
				SCFResourceInputData resInputData;
				resInputData.provider = std::move( pDataProvider );
				resInputData.dataType = SCFResourceInputDataType::DataProvider;

				return _setResourceInputData( pResource, std::move( resInputData ) );
			}
		}

		return false;
	}

	bool SCFIndexBuilder::setResourceDataSource( const SCFResourceTemplate * pResource,
												 SCFResourceInputData::DataSource pDataSource )
	{
		if( pResource && pDataSource.dataPtr && ( pDataSource.dataSize > 0 ) )
		{
			SCFResourceInputData resInputData;
			resInputData.source = std::move( pDataSource );
			resInputData.dataType = SCFResourceInputDataType::DataSource;

			return _setResourceInputData( pResource, std::move( resInputData ) );
		}

		return false;
	}

	const SCFVirtualFolderTemplate & SCFIndexBuilder::getRootVirtualFolder() const
	{
		return _privateWorkingData->rootFolder;
	}

	bool SCFIndexBuilder::checkNameAndUID( const std::string & pName, const std::string & pUID )
	{
		if( pName.empty() )
		{
			return false;
		}

		const auto separatorPos = pName.find_first_of( "/\\", 0 );
		if( separatorPos != std::string::npos )
		{
			return false;
		}

		if( !pUID.empty() )
		{
			const auto existingUIDIter = _privateWorkingData->globalUIDSet.find( pUID );
			if( existingUIDIter != _privateWorkingData->globalUIDSet.end() )
			{
				return false;
			}
		}

		return true;
	}

	SCFVirtualFolderTemplate * SCFIndexBuilder::_findFolderInternal( const std::string & pFolderLocation,
																	 SCFVirtualFolderTemplate * pRootFolder )
	{
		if( !pRootFolder )
		{
			pRootFolder = &( _privateWorkingData->rootFolder );
		}

		SCFVirtualFolderTemplate * currentFolderPtr = pRootFolder;

		for( auto pathNameIterator = PathNameIterator( pFolderLocation ); !pathNameIterator.empty(); ++pathNameIterator )
		{
			// Iterator yields a name of a next sub-folder.
			const auto & subFolderName = pathNameIterator.name();

			// Get the sub-folder from the sub-folder list. If there is no such thing,
			// it means the path specified is invalid. Simply return nullptr in such case.
			const auto subFolderIter = currentFolderPtr->subFolderList.find( subFolderName );

			if( subFolderIter == currentFolderPtr->subFolderList.end() )
			{
				currentFolderPtr = nullptr;
				break;
			}

			// Set the folder pointer to this sub-folder and continue.
			currentFolderPtr = &( *subFolderIter );
		}

		// Either a valid folder with a requested path or nullptr.
		return currentFolderPtr;
	}

	SCFResourceTemplate * SCFIndexBuilder::_findResourceInternal( const std::string & pResourceLocation,
																  SCFVirtualFolderTemplate * pRootFolder )
	{
		if( !pRootFolder )
		{
			pRootFolder = &( _privateWorkingData->rootFolder );
		}

		SCFVirtualFolderTemplate * currentFolderPtr = pRootFolder;
		SCFResourceTemplate * resourcePtr = nullptr;

		for( auto pathNameIterator = PathNameIterator( pResourceLocation ); !pathNameIterator.empty(); ++pathNameIterator )
		{
			// Iterator yields a name of a next sub-folder or a resource.
			const auto & subLocationName = pathNameIterator.name();

			// Check if there are still some path components to be iterated over. If so, the current
			// one refers to a sub-folder. Otherwise, (the last path component) - it is a resource name.
			if( pathNameIterator.hasNext() )
			{
				const auto subFolderIter = currentFolderPtr->subFolderList.find( subLocationName );
				if( subFolderIter == currentFolderPtr->subFolderList.end() )
				{
					break;
				}

				currentFolderPtr = &( *subFolderIter );
			}
			else
			{
				const auto resourceIter = pRootFolder->resourceList.find( subLocationName );
				if( resourceIter == pRootFolder->resourceList.end() )
				{
					break;
				}

				resourcePtr = &( *resourceIter );
			}
		}

		return resourcePtr;
	}

	bool SCFIndexBuilder::_setResourceInputData( const SCFResourceTemplate * pResource, SCFResourceInputData pInputData )
	{
		if( !pResource || !pInputData )
		{
			return false;
		}

		auto * resourcePtr = _findResourceInternal( pResource->path );
		if( !resourcePtr )
		{
			return false;
		}

		resourcePtr->inputData = std::move( pInputData );

		return true;
	}

	void SCFIndexBuilder::processLocationString( std::string & pLocationString )
	{
		if( pLocationString.empty() )
		{
			return;
		}

		auto pathNameIterator = PathNameIterator( std::move( pLocationString ) );

		pLocationString = std::string{};

		while( !pathNameIterator.empty() )
		{
			if( !pLocationString.empty() )
			{
				pLocationString += "/";
			}
			pLocationString += pathNameIterator.name();
		}
	}

}
