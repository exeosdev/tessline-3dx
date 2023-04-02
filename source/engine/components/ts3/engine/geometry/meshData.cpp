
#include "meshData.h"
#include "geometryDataFormat.h"
#include <ts3/engine/res/image/pngCommon.h>


namespace ts3
{

	void MeshData::initializeStorage( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		setContainerCapacity( pVertexElementsNum, pIndexElementsNum );

		for( uint32 iVertexStream = 0; iVertexStream < mDataFormat.activeVertexStreamIndexEnd(); ++iVertexStream )
		{
			if( mDataFormat.isVertexStreamActive( iVertexStream ) )
			{
				const auto vertexStreamElementSize = mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
				const auto vertexStreamDataBufferSize = pVertexElementsNum * vertexStreamElementSize;
				_vertexDataBuffers[iVertexStream].resize( vertexStreamDataBufferSize );
				_allGeometryDataRef.vertexStreamDataRegions[iVertexStream].dataPtr = _vertexDataBuffers[iVertexStream].data();
			}
		}

		if( pIndexElementsNum > 0 )
		{
			const auto _indexDataBufferSize = pIndexElementsNum * mDataFormat.indexElementSizeInBytes();
			_indexDataBuffer.resize( _indexDataBufferSize );
			_allGeometryDataRef.indexDataRegion.dataPtr = _indexDataBuffer.data();
		}
	}

	void MeshData::setMeshName( std::string pName )
	{
		_meshName = std::move( pName );
	}

	MeshSubComponentData * MeshData::addMeshComponent( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		if( !checkStorageAvailableSpace( pVertexElementsNum, pIndexElementsNum ) )
		{
			return nullptr;
		}

		auto * currentLastSubComponent = !_meshSubComponents.empty() ? &_meshSubComponents.back() : nullptr;

		auto & subComponentData = _meshSubComponents.emplace_back( mDataFormat );
		subComponentData.componentIndex = numeric_cast<uint32>( _meshSubComponents.size() - 1 );

		if( !currentLastSubComponent )
		{
			subComponentData.geometryDataRef = gmutil::getGeometryDataReferenceSubRegion(
					_allGeometryDataRef,
					0, pVertexElementsNum,
					0, pIndexElementsNum );
		}
		else
		{
			subComponentData.geometryDataRef = gmutil::advanceGeometryDataReference(
					currentLastSubComponent->geometryDataRef,
					pVertexElementsNum,
					pIndexElementsNum );
		}

		updateStoredGeometrySize( pVertexElementsNum, pIndexElementsNum );

		return &( _meshSubComponents.back() );
	}

	DataBufferRegionSubElementMappingReadWrite MeshData::getIndexDataSubRegionReadWrite(
			const CPUGeometryDataReference & pMeshDataRef ) noexcept
	{
		return DataBufferRegionSubElementMappingReadWrite {
				_indexDataBuffer.data() + ( pMeshDataRef.indexDataRegion.offsetInElementsNum * pMeshDataRef.indexDataRegion.elementSize ),
				mDataFormat.indexElementSizeInBytes(),
				mDataFormat.indexElementSizeInBytes()
		};
	}

	DataBufferRegionSubElementMappingReadWrite MeshData::getVertexAttributeDataSubRegionReadWrite(
			const CPUGeometryDataReference & pMeshDataRef,
			uint32 pAttributeIndex ) noexcept
	{
		const auto & attributeFormat = mDataFormat.attribute( pAttributeIndex );
		const auto & vertexStreamDataRef = pMeshDataRef.vertexStreamDataRegions[attributeFormat.streamIndex];

		auto * bufferBasePtr = _vertexDataBuffers[attributeFormat.streamIndex].data();

		return DataBufferRegionSubElementMappingReadWrite {
				bufferBasePtr + ( vertexStreamDataRef.offsetInElementsNum * vertexStreamDataRef.elementSize ),
				attributeFormat.attributeTotalSizeInBytes,
				vertexStreamDataRef.elementSize
		};
	}


	MeshData & MeshGroupData::addMeshData( std::unique_ptr<MeshData> pMeshData )
	{
		_geometrySize.append( pMeshData->getGeometrySize() );
		_meshDataArray.push_back( std::move( pMeshData ) );

		return *( _meshDataArray.back() );
	}

	MeshData & MeshGroupData::addMeshData( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		auto meshData = std::make_unique<MeshData>( mGeometryDataFormat );
		meshData->initializeStorage( pVertexElementsNum, pIndexElementsNum );

		return addMeshData( std::move( meshData ) );
	}

}
