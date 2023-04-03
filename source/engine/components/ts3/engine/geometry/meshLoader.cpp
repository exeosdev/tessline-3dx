
#include "meshLoader.h"
#include "meshGroup.h"
#include "geometryStorage.h"
#include "geometryManager.h"
#include "geometryDataTransfer.h"

namespace ts3
{

	std::unique_ptr<MeshGroup> MeshLoader::importMeshGroup(
			const MeshImportContext & pImportContext,
			std::string pGroupName,
			const std::vector<MeshInputDesc> & pGroupDesc )
	{
		auto meshGroupData = std::make_unique<MeshGroupData>( *pImportContext.geometryDataFormat  );

		for( const auto & meshInputDesc : pGroupDesc )
		{
			if( auto meshData = pImportContext.importer->importMesh( meshInputDesc.sourceFilename, *pImportContext.geometryDataFormat ) )
			{
				meshData->setMeshName( meshInputDesc.meshName );
				meshGroupData->addMeshData( std::move( meshData ) );
			}
			else
			{
				ts3DebugOutputFmt( "Failed to import mesh: %s", meshInputDesc.sourceFilename.c_str() );
			}
		}

		ts3DebugOutputFmt(
				"Imported -%u- meshes out of -%u- specified",
				( uint32 )meshGroupData->getMeshesNum(),
				( uint32 )pGroupDesc.size() );

		auto * meshGroupGeometryStorage = createGeometryStorageForMeshGroup( *pImportContext.geometryManager , *pImportContext.geometryDataFormat , *meshGroupData );
		if( !meshGroupGeometryStorage )
		{
			return nullptr;
		}

		auto meshGroup = std::make_unique<MeshGroup>( pGroupName, meshGroupGeometryStorage );
		for( uint32 iMeshIndex = 0; iMeshIndex < meshGroupData->getMeshesNum(); ++iMeshIndex )
		{
			const auto * meshData = meshGroupData->getMeshData( iMeshIndex );
			auto * mesh = meshGroup->addMesh( meshData->getMeshName() );

			for( uint32 iMeshComponent = 0; iMeshComponent < meshData->getMeshSubComponentsNum(); ++iMeshComponent )
			{
				const auto * meshSubComponentData = meshData->getMeshSubComponentData( iMeshComponent );
				const auto meshSubComponentGeometrySize = meshSubComponentData->geometryDataRef.calculateGeometrySize();
				auto meshSubComponentGeometryRef = meshGroupGeometryStorage->addIndexedGeometry( meshSubComponentGeometrySize.vertexElementsNum, meshSubComponentGeometrySize.indexElementsNum );
				mesh->addMeshComponent( meshSubComponentData->name, meshSubComponentGeometryRef );
			}

			pImportContext.geometryDataTransfer->initializeMeshData( *mesh->geometryDataRef(), meshData->getAllGeometryDataRef() );
		}

		return meshGroup;
	}

	GeometryStorage * MeshLoader::createGeometryStorageForMeshGroup(
			GeometryManager & pGeometryManager,
			const GeometryDataFormat & pGeometryDataFormat,
			const MeshGroupData & pMeshGroupData )
	{
		GeometryStorageCreateInfo storageCreateInfo;
		storageCreateInfo.dataFormat = &pGeometryDataFormat;
		storageCreateInfo.capacity.vertexDataCapacityInElementsNum = pMeshGroupData.geometrySize().vertexElementsNum;
		storageCreateInfo.capacity.indexDataCapacityInElementsNum = pMeshGroupData.geometrySize().indexElementsNum;
		storageCreateInfo.commonBufferUsagePolicy = EGPUBufferUsagePolicy::GeometryBufferStatic;

		return pGeometryManager.createGeometryStorage( storageCreateInfo );
	}

}
