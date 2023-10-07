
#include "geometryContainer.h"

namespace ts3
{

	GeometryContainer::GeometryContainer()
	{}

	GeometryContainer::GeometryContainer(
			const GeometryDataFormatBase & pDataFormat,
			GeometryVertexBufferReference * pVertexBufferRefsPtr )
	: _activeVertexStreamsNum( pDataFormat.activeVertexStreamsNum() )
	, _vertexBufferRefsPtr( pVertexBufferRefsPtr )
	{}

	bool GeometryContainer::setIndexBuffer( const GeometryIndexBufferReference & pIndexBufferReference )
	{
		_indexBufferRefPtr = pIndexBufferReference;
		return true;
	}

	bool GeometryContainer::setVertexBuffer( uint32 pBufferIndex, const GeometryVertexBufferReference & pVertexBufferReference )
	{
		if( !_vertexBufferRefsPtr || ( pBufferIndex >= _activeVertexStreamsNum ) )
		{
			return false;
		}

		_vertexBufferRefsPtr[pBufferIndex] = pVertexBufferReference;
		return true;
	}

	void GeometryContainer::setVertexBufferRefsStorage( GeometryVertexBufferReference * pVertexBufferRefsPtr )
	{
		_vertexBufferRefsPtr = pVertexBufferRefsPtr;
	}


	/*
	GeometryContainer::GeometryContainer( const GeometryDataFormatBase & pDataFormat )
	: mDataFormat( pDataFormat )
	, _allGeometryDataRef( pDataFormat )
	{}

	const GeometryDataReferenceBase & GeometryContainer::getAllGeometryDataRef() const noexcept
	{
		return _allGeometryDataRef;
	}

	const GeometryContainerStorageMetrics & GeometryContainer::getStorageMetrics() const noexcept
	{
		return _storageMetrics;
	}

	const GeometrySize & GeometryContainer::getGeometrySize() const noexcept
	{
		return _storageMetrics.geometrySize;
	}

	bool GeometryContainer::isStorageInitialized() const noexcept
	{
		return _storageMetrics.capacity.vertexDataCapacityInElementsNum > 0;
	}

	bool GeometryContainer::isIndexedGeometryContainer() const noexcept
	{
		return _storageMetrics.capacity.indexDataCapacityInElementsNum > 0;
	}

	bool GeometryContainer::checkStorageAvailableSpace( uint32 pVertexElementsNum, uint32 pIndexElementsNum ) const noexcept
	{
		const auto maxVertexDataElementsNum =
				_storageMetrics.capacity.vertexDataCapacityInElementsNum - _storageMetrics.vertexDataAllocationOffsetInElementsNum;

		const auto maxIndexDataElementsNum =
				_storageMetrics.capacity.indexDataCapacityInElementsNum - _storageMetrics.indexDataAllocationOffsetInElementsNum;

		return ( pVertexElementsNum <= maxVertexDataElementsNum ) && ( pIndexElementsNum <= maxIndexDataElementsNum );
	}

	void GeometryContainer::setContainerCapacity( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		_storageMetrics.capacity.vertexDataCapacityInElementsNum = pVertexElementsNum;
		_storageMetrics.capacity.indexDataCapacityInElementsNum = pIndexElementsNum;

		for( auto iVertexStream : mDataFormat.activeVertexStreams() )
		{
			const auto vertexStreamElementSize = mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
			_allGeometryDataRef.vertexStreamDataRegions[iVertexStream].elementSize = vertexStreamElementSize;
			_allGeometryDataRef.vertexStreamDataRegions[iVertexStream].offsetInElementsNum = 0;
			_allGeometryDataRef.vertexStreamDataRegions[iVertexStream].sizeInElementsNum = pVertexElementsNum;
		}

		if( mDataFormat.isIndexedGeometry() && ( pIndexElementsNum > 0 ) )
		{
			const auto indexDataBufferSize = pIndexElementsNum * mDataFormat.indexElementSizeInBytes();
			_allGeometryDataRef.indexDataRegion.elementSize = mDataFormat.indexElementSizeInBytes();
			_allGeometryDataRef.indexDataRegion.offsetInElementsNum = 0;
			_allGeometryDataRef.indexDataRegion.sizeInElementsNum = pIndexElementsNum;
		}
	}

	void GeometryContainer::updateStoredGeometrySize( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		_storageMetrics.geometrySize.vertexElementsNum += pVertexElementsNum;
		_storageMetrics.geometrySize.indexElementsNum += pIndexElementsNum;
	}
	 */

}
