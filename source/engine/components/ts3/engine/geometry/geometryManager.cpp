
#include "geometryManager.h"
#include "geometryStorage.h"

namespace ts3
{

	GeometryManager::GeometryManager( const CoreEngineState & pCES )
	: GPUResourceContainer(  pCES )
	{}

	GeometryManager::~GeometryManager() = default;

	GeometryStorage * GeometryManager::createGeometryStorage(
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage * pSharedStorage )
	{
		return createStorageInternal( pCreateInfo, pSharedStorage );
	}

	GeometryStorage * GeometryManager::createNamedGeometryStorage(
			std::string pUniqueStorageName,
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage * pSharedStorage )
	{
		if( auto * existingStorage = getStorageByName( pUniqueStorageName ) )
		{
			return nullptr;
		}

		auto * geometryStorage = createStorageInternal( pCreateInfo, pSharedStorage );
		if( geometryStorage )
		{
			_namedStoragesMap.insert( { std::move( pUniqueStorageName ), geometryStorage } );
		}

		return geometryStorage;
	}

	GeometryStorage * GeometryManager::getStorageByIndex( uint32 pStorageIndex ) const noexcept
	{
		return ( pStorageIndex < _storages.size() ) ? _storages.back().get() : nullptr;
	}

	GeometryStorage * GeometryManager::getStorageByName( const std::string & pStorageName ) const noexcept
	{
		const auto storageIter = _namedStoragesMap.find( pStorageName );
		return ( storageIter != _namedStoragesMap.end() ) ? storageIter->second : nullptr;
	}

	GeometryStorage * GeometryManager::createStorageInternal(
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage * pSharedStorage )
	{
		auto geometryStorage = GeometryStorage::createStorage( mCES, pCreateInfo, pSharedStorage );
		if( geometryStorage )
		{
			_storages.push_back( std::move( geometryStorage ) );
		}

		return _storages.back().get();
	}

}
