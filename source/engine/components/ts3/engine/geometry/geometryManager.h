
#pragma once

#ifndef __TS3_ENGINE_GEOMETRY_MANAGER_H__
#define __TS3_ENGINE_GEOMETRY_MANAGER_H__

#include "geometryCommonDefs.h"
#include "../geometry/geometryReference.h"
#include "../geometry/geometryVertexFormat.h"

namespace ts3
{

	class GPUResourceContainer : public CoreEngineObject
	{
	public:
		GPUResourceContainer( const CoreEngineState & pCES )
		: CoreEngineObject( pCES )
		{}
	};

	class GeometryManager : public GPUResourceContainer
	{
	public:
		GeometryManager( const CoreEngineState & pCES );
		virtual ~GeometryManager();

		GeometryStorage * createGeometryStorage(
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage * pSharedStorage = nullptr );

		GeometryStorage * createNamedGeometryStorage(
				std::string pUniqueStorageName,
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage * pSharedStorage = nullptr );

		GeometryStorage * getStorageByIndex( uint32 pStorageIndex ) const noexcept;

		GeometryStorage * getStorageByName( const std::string & pStorageName ) const noexcept;

	private:
		GeometryStorage * createStorageInternal(
				const GeometryStorageCreateInfo & pCreateInfo,
				const GeometryStorage * pSharedStorage );

	private:
		using GeometryStoragePtr = std::unique_ptr<GeometryStorage>;
		using GeometryStorageList = std::vector<GeometryStoragePtr>;
		using NamedGeometryStorageMap = std::map<std::string, GeometryStorage *>;

		GeometryStorageList _storages;

		NamedGeometryStorageMap _namedStoragesMap;
	};

} // namespace ts3

#endif // __TS3_ENGINE_GEOMETRY_MANAGER_H__
