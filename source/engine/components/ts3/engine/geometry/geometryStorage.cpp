
#include "geometryStorage.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources//gpuBuffer.h>

namespace ts3
{

	GeometryStorage::GeometryStorage( const GeometryDataFormat & pDataFormat )
	: GeometryContainer( pDataFormat )
	{}

	GeometryStorage::~GeometryStorage() = default;

	gpuapi::IAVertexStreamImmutableStateHandle GeometryStorage::getGpaVertexStreamState() const noexcept
	{
		return _gpaVertexStreamState;
	}

	gpuapi::GPUBufferHandle GeometryStorage::getIndexBuffer() const noexcept
	{
		return _indexBuffer;
	}

	gpuapi::GPUBufferHandle GeometryStorage::getVertexBuffer( uint32 pIndex ) const noexcept
	{
		return ( pIndex < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM ) ? _vertexBufferArray[pIndex] : nullptr;
	}

	GeometryReference * GeometryStorage::addIndexedGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		if( ( pVertexElementsNum == 0 ) || ( pIndexElementsNum == 0 ) || !isIndexedGeometryContainer() )
		{
			return nullptr;
		}

		return addGeometry( pVertexElementsNum, pIndexElementsNum );
	}

	GeometryReference * GeometryStorage::addNonIndexedGeometry( uint32 pVertexElementsNum )
	{
		if( pVertexElementsNum == 0 )
		{
			return nullptr;
		}

		return addGeometry( pVertexElementsNum, 0 );
	}

	GeometryStoragePtr GeometryStorage::createStorage(
			const CoreEngineState & pCES,
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage * pSharedStorage )
	{
		if( pSharedStorage && !validateSharedBuffersConfiguration( pCreateInfo, *pSharedStorage ) )
		{
			return nullptr;
		}

		auto geometryStorage = std::make_unique<GeometryStorage>( *pCreateInfo.dataFormat );

		geometryStorage->createStorageGPUBuffers( pCES, pCreateInfo );

		if( pSharedStorage )
		{
			geometryStorage->bindSharedGPUBuffers( pCreateInfo, *pSharedStorage );
		}

		geometryStorage->initializeVertexStreamState( pCES );

		return geometryStorage;
	}

	void GeometryStorage::createStorageGPUBuffers( const CoreEngineState & pCES, const GeometryStorageCreateInfo & pCreateInfo )
	{
		setContainerCapacity( pCreateInfo.capacity.vertexDataCapacityInElementsNum, pCreateInfo.capacity.indexDataCapacityInElementsNum );

		for( auto iVertexStream : mDataFormat.activeVertexStreams() )
		{
			if( pCreateInfo.dataFormat->isVertexStreamActive( iVertexStream ) )
			{
				if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::AllocLocal )
				{
					const auto & vertexStreamFormat = pCreateInfo.dataFormat->vertexStream( iVertexStream );

					const auto bufferSizeInBytes =
							vertexStreamFormat.elementSizeInBytes * pCreateInfo.capacity.vertexDataCapacityInElementsNum;

					const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
							pCreateInfo.vertexBufferDescArray[iVertexStream].bufferUsagePolicy,
							pCreateInfo.commonBufferUsagePolicy );

					_vertexBufferArray[iVertexStream] = createVertexBuffer(
							pCES,
							bufferSizeInBytes,
							bufferUsagePolicy );

					_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iVertexStream ) );
				}
			}
		}

		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::AllocLocal )
		{
			if( pCreateInfo.dataFormat->isIndexedGeometry() )
			{
				const auto bufferSizeInBytes =
						pCreateInfo.dataFormat->indexElementSizeInBytes() * pCreateInfo.capacity.indexDataCapacityInElementsNum;

				const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
						pCreateInfo.indexBufferDesc.bufferUsagePolicy,
						pCreateInfo.commonBufferUsagePolicy );

				_indexBuffer = createIndexBuffer(
						pCES,
						bufferSizeInBytes,
						bufferUsagePolicy );

				_vertexStreamBindingMask.set( gpuapi::E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
			}
		}
	}

	void GeometryStorage::bindSharedGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo, const GeometryStorage & pSharedStorage )
	{
		for( auto iVertexStream : mDataFormat.activeVertexStreams() )
		{
			if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				_vertexBufferArray[iVertexStream] = pSharedStorage.getVertexBuffer( iVertexStream );
				_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iVertexStream ) );
			}
		}

		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::ShareExternal )
		{
			_indexBuffer = pSharedStorage.getIndexBuffer();
			_vertexStreamBindingMask.set( gpuapi::E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		}
	}

	void GeometryStorage::initializeVertexStreamState( const CoreEngineState & pCES )
	{
		gpuapi::IAVertexStreamDefinition vertexStreamDefinition;
		vertexStreamDefinition.activeBindingsMask = _vertexStreamBindingMask;

		for( auto iVertexStream : mDataFormat.activeVertexStreams() )
		{
			if( const auto & vertexBuffer = _vertexBufferArray[iVertexStream] )
			{
				auto & vertexBufferReference = vertexStreamDefinition.vertexBufferReferences[iVertexStream];
				vertexBufferReference.sourceBuffer = vertexBuffer;
				vertexBufferReference.relativeOffset = 0;
				vertexBufferReference.vertexStride = mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
			}
		}

		if( const auto & indexBuffer = _indexBuffer )
		{
			auto & indexBufferReference = vertexStreamDefinition.indexBufferReference;
			indexBufferReference.sourceBuffer = indexBuffer;
			indexBufferReference.relativeOffset = 0;
			indexBufferReference.indexFormat = mDataFormat.indexDataFormat();
		}

		_gpaVertexStreamState = pCES.mGPUDevice->createIAVertexStreamImmutableState( vertexStreamDefinition );
	}

	GeometryReference * GeometryStorage::addGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		if( !checkStorageAvailableSpace( pVertexElementsNum, pIndexElementsNum ) )
		{
			return nullptr;
		}

		auto & geometryReference = _geometryRefList.emplace_back( *this );
		geometryReference.geometryIndex = numeric_cast<uint32>( _geometryRefList.size() - 1 );

		if( geometryReference.geometryIndex == 0 )
		{
			geometryReference.dataReference = gmutil::getGeometryDataReferenceSubRegion(
					_allGeometryDataRef,
					0, pVertexElementsNum,
					0, pIndexElementsNum );
		}
		else
		{
			const auto & previousGeometryRef = _geometryRefList[geometryReference.geometryIndex - 1];
			geometryReference.dataReference = gmutil::advanceGeometryDataReference(
					previousGeometryRef.dataReference,
					pVertexElementsNum,
					pIndexElementsNum );
		}

		updateStoredGeometrySize( pVertexElementsNum, pIndexElementsNum );

		_storageMetrics.indexDataAllocationOffsetInElementsNum += pIndexElementsNum;
		_storageMetrics.vertexDataAllocationOffsetInElementsNum += pVertexElementsNum;

		return &geometryReference;
	}

	gpuapi::GPUBufferHandle GeometryStorage::createIndexBuffer(
			const CoreEngineState & pCES,
			gpuapi::gpu_memory_size_t pBufferSize,
			EGPUBufferUsagePolicy pUsagePolicy )
	{
		gpuapi::GPUBufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.bufferSize = pBufferSize;
		bufferCreateInfo.memoryFlags = cxdefs::getGPUBufferUsagePolicyMemoryFlags( pUsagePolicy );
		bufferCreateInfo.resourceFlags = cxdefs::getGPUBufferUsagePolicyResourceFlags( pUsagePolicy );
		bufferCreateInfo.resourceFlags.unset( gpuapi::E_GPU_BUFFER_BIND_MASK_ALL );
		bufferCreateInfo.resourceFlags.set( gpuapi::E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT );

		auto indexBuffer = pCES.mGPUDevice->createGPUBuffer( bufferCreateInfo );
		ts3DebugAssert( indexBuffer );

		return indexBuffer;
	}

	gpuapi::GPUBufferHandle GeometryStorage::createVertexBuffer(
			const CoreEngineState & pCES,
			gpuapi::gpu_memory_size_t pBufferSize,
			EGPUBufferUsagePolicy pUsagePolicy )
	{
		gpuapi::GPUBufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.bufferSize = pBufferSize;
		bufferCreateInfo.memoryFlags = cxdefs::getGPUBufferUsagePolicyMemoryFlags( pUsagePolicy );
		bufferCreateInfo.resourceFlags = cxdefs::getGPUBufferUsagePolicyResourceFlags( pUsagePolicy );
		bufferCreateInfo.resourceFlags.unset( gpuapi::E_GPU_BUFFER_BIND_MASK_ALL );
		bufferCreateInfo.resourceFlags.set( gpuapi::E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT );

		auto vertexBuffer = pCES.mGPUDevice->createGPUBuffer( bufferCreateInfo );
		ts3DebugAssert( vertexBuffer );

		return vertexBuffer;
	}

	bool GeometryStorage::validateSharedBuffersConfiguration(
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage & pSharedStorage )
	{
		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::ShareExternal )
		{
			const auto indexBuffer = pSharedStorage.getIndexBuffer();
			if( !indexBuffer )
			{
				return false;
			}

			const auto sharedIndexDataCapacity = pSharedStorage.getStorageMetrics().capacity.indexDataCapacityInElementsNum;
			const auto requiredIndexDataCapacity = pCreateInfo.capacity.indexDataCapacityInElementsNum;
			if( sharedIndexDataCapacity != requiredIndexDataCapacity )
			{
				return false;
			}

			const auto sharedIndexElementSize = pSharedStorage.mDataFormat.indexElementSizeInBytes() ;
			const auto requiredIndexElementSize = pCreateInfo.dataFormat->indexElementSizeInBytes() ;
			if( sharedIndexElementSize != requiredIndexElementSize )
			{
				return false;
			}
		}

		bool vertexBufferShared = false;

		for( auto iVertexStream : pCreateInfo.dataFormat->activeVertexStreams() )
		{
			if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				vertexBufferShared = true;

				const auto vertexBuffer = pSharedStorage.getVertexBuffer( iVertexStream );
				if( !vertexBuffer )
				{
					return false;
				}

				const auto sharedVertexStreamElementSize = pSharedStorage.mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
				const auto requiredVertexStreamElementSize = pCreateInfo.dataFormat->vertexStreamElementSizeInBytes( iVertexStream );
				if( sharedVertexStreamElementSize != requiredVertexStreamElementSize )
				{
					return false;
				}
			}
		}

		if( vertexBufferShared )
		{
			const auto sharedVertexDataCapacity = pSharedStorage.getStorageMetrics().capacity.vertexDataCapacityInElementsNum;
			const auto requiredVertexDataCapacity = pCreateInfo.capacity.vertexDataCapacityInElementsNum;
			if( sharedVertexDataCapacity != requiredVertexDataCapacity )
			{
				return false;
			}
		}

		return true;
	}

	EGPUBufferUsagePolicy GeometryStorage::resolveGeometryBufferUsagePolicy(
			EGPUBufferUsagePolicy pBufferUsagePolicy,
			EGPUBufferUsagePolicy pCommonUsagePolicy )
	{
		auto bufferUsagePolicy = pBufferUsagePolicy;
		if( bufferUsagePolicy == EGPUBufferUsagePolicy::Undefined )
		{
			if( pCommonUsagePolicy != EGPUBufferUsagePolicy::Undefined )
			{
				bufferUsagePolicy = pCommonUsagePolicy;
			}
			else
			{
				bufferUsagePolicy = EGPUBufferUsagePolicy::GeometryBufferStatic;
			}
		}
		return bufferUsagePolicy;
	}

}
