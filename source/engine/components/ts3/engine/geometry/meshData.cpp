
#include "meshData.h"
#include "geometryDataFormat.h"
#include <ts3/engine/res/image/pngCommon.h>


namespace ts3
{

	DataBufferRegionSubElementRefReadWrite MeshData::getIndexDataSubRegionReadWrite(
			const CPUGeometryDataReferenceBase & pMeshDataRef ) noexcept
	{
		return DataBufferRegionSubElementRefReadWrite {
				_indexDataBuffer.data() + ( pMeshDataRef.indexDataRegion.offsetInElementsNum * pMeshDataRef.indexDataRegion.elementSize ),
				mDataFormat.indexElementSizeInBytes(),
				mDataFormat.indexElementSizeInBytes()
		};
	}

	DataBufferRegionSubElementRefReadWrite MeshData::getVertexAttributeDataSubRegionReadWrite(
			const CPUGeometryDataReferenceBase & pMeshDataRef,
			uint32 pAttributeIndex ) noexcept
	{
		const auto & attributeFormat = mDataFormat.attribute( pAttributeIndex );
		const auto & vertexStreamDataRef = pMeshDataRef.vertexStreamDataRegions[attributeFormat.streamIndex];

		auto * bufferBasePtr = _vertexDataBuffers[attributeFormat.streamIndex].data();

		return DataBufferRegionSubElementRefReadWrite {
				bufferBasePtr + ( vertexStreamDataRef.offsetInElementsNum * vertexStreamDataRef.elementSize ) + attributeFormat.streamElementRelativeOffset,
				attributeFormat.attributeTotalSizeInBytes,
				vertexStreamDataRef.elementSize
		};
	}

	void MeshData::initializeStorage( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		setContainerCapacity( pVertexElementsNum, pIndexElementsNum );

		for( auto iVertexStream : mDataFormat.activeVertexStreams() )
		{
			const auto vertexStreamElementSize = mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
			const auto vertexStreamDataBufferSize = pVertexElementsNum * vertexStreamElementSize;
			_vertexDataBuffers[iVertexStream].resize( vertexStreamDataBufferSize );
			_allGeometryDataRef.vertexStreamDataRegions[iVertexStream].dataPtr = _vertexDataBuffers[iVertexStream].data();
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

		auto & subComponentData = _meshSubComponents.emplace_back( mDataFormat );
		subComponentData.componentIndex = numeric_cast<uint32>( _meshSubComponents.size() - 1 );

		if( subComponentData.componentIndex == 0 )
		{
			subComponentData.geometryDataRef = gmutil::getGeometryDataReferenceBaseSubRegion(
					_allGeometryDataRef,
					0, pVertexElementsNum,
					0, pIndexElementsNum );
		}
		else
		{
			const auto & previousSubComponent = _meshSubComponents[subComponentData.componentIndex - 1];
			subComponentData.geometryDataRef = gmutil::advanceGeometryDataReferenceBase(
					previousSubComponent.geometryDataRef,
					pVertexElementsNum,
					pIndexElementsNum );
		}

		updateStoredGeometrySize( pVertexElementsNum, pIndexElementsNum );

		return &( _meshSubComponents.back() );
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
