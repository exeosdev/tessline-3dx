
#include "geometryStorage.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources//gpuBuffer.h>

namespace ts3
{

	GeometryStorage::GeometryStorage(
			const CoreEngineState & pCES,
			const GeometryDataFormat & pDataFormat,
			const GeometryStorageMetrics & pStorageMetrics )
	: CoreEngineObject( pCES )
	, mDataFormat( pDataFormat )
	, mStorageMetrics( pStorageMetrics )
	{}

	GeometryStorage::~GeometryStorage() = default;

	gpuapi::IAVertexStreamImmutableStateHandle GeometryStorage::getGpaVertexStreamState() const noexcept
	{
		return _gpaVertexStreamState;
	}

	gpuapi::GPUBufferHandle GeometryStorage::getIndexBuffer() const noexcept
	{
		return _indexBufferState.gpuBuffer;
	}

	gpuapi::GPUBufferHandle GeometryStorage::getVertexBuffer( uint32 pIndex ) const noexcept
	{
		return ( pIndex < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM ) ? _vertexBufferStateArray[pIndex].gpuBuffer : nullptr;
	}

	const GeometryReference & GeometryStorage::getAllGeometryReference() const noexcept
	{
		return _allGeometryReference;
	}

	GeometryReference * GeometryStorage::addIndexedGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		if( ( pVertexElementsNum == 0 ) || ( pIndexElementsNum == 0 ) || !_indexBufferState.gpuBuffer )
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

	std::unique_ptr<GeometryStorage> GeometryStorage::createStorage(
			const CoreEngineState & pCES,
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage * pSharedStorage )
	{
		if( pSharedStorage && !validateSharedBuffersConfiguration( pCreateInfo, *pSharedStorage ) )
		{
			return nullptr;
		}

		auto geometryStorage = std::make_unique<GeometryStorage>(
				pCES,
				*pCreateInfo.dataFormat,
				pCreateInfo.metrics );

		geometryStorage->createStorageGPUBuffers( pCreateInfo );

		if( pSharedStorage )
		{
			geometryStorage->bindSharedGPUBuffers( pCreateInfo, *pSharedStorage );
		}

		return geometryStorage;
	}

	void GeometryStorage::createStorageGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo )
	{
		_allGeometryReference.storage = this;
		_allGeometryReference.dataFormat = &mDataFormat;
		_allGeometryReference.geometryIndex = CX_UINT32_MAX;

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( pCreateInfo.dataFormat->isVertexStreamActive( iVertexStream ) )
			{
				if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::AllocLocal )
				{
					const auto & vertexStreamFormat = pCreateInfo.dataFormat->vertexStream( iVertexStream );

					const auto bufferSizeInBytes =
							vertexStreamFormat.elementSizeInBytes * pCreateInfo.metrics.vertexDataCapacityInElementsNum;

					const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
							pCreateInfo.vertexBufferDescArray[iVertexStream].bufferUsagePolicy,
							pCreateInfo.commonBufferUsagePolicy );

					_vertexBufferStateArray[iVertexStream].gpuBuffer = createVertexBuffer(
							mCES,
							bufferSizeInBytes,
							bufferUsagePolicy );

					_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iVertexStream ) );

					auto & allGeometryVertexStreamDataRegion = _allGeometryReference.dataReference.vertexStreamDataRegions[iVertexStream];
					allGeometryVertexStreamDataRegion.elementSize = vertexStreamFormat.elementSizeInBytes;
					allGeometryVertexStreamDataRegion.offsetInElementsNum = 0;
					allGeometryVertexStreamDataRegion.sizeInElementsNum = pCreateInfo.metrics.vertexDataCapacityInElementsNum;
				}
			}
		}

		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::AllocLocal )
		{
			if( pCreateInfo.dataFormat->isIndexedGeometry() )
			{
				const auto bufferSizeInBytes =
						pCreateInfo.dataFormat->indexElementSizeInBytes() * pCreateInfo.metrics.vertexDataCapacityInElementsNum;

				const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
						pCreateInfo.indexBufferDesc.bufferUsagePolicy,
						pCreateInfo.commonBufferUsagePolicy );

				_indexBufferState.gpuBuffer = createIndexBuffer(
						mCES,
						bufferSizeInBytes,
						bufferUsagePolicy );

				_vertexStreamBindingMask.set( gpuapi::E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );

				auto & allGeometryIndexDataRegion = _allGeometryReference.dataReference.indexDataRegion;
				allGeometryIndexDataRegion.elementSize = pCreateInfo.dataFormat->indexElementSizeInBytes();
				allGeometryIndexDataRegion.offsetInElementsNum = 0;
				allGeometryIndexDataRegion.sizeInElementsNum = pCreateInfo.metrics.indexDataCapacityInElementsNum;
			}
		}
	}

	void GeometryStorage::bindSharedGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo, const GeometryStorage & pSharedStorage )
	{
		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				_vertexBufferStateArray[iVertexStream].gpuBuffer = pSharedStorage.getVertexBuffer( iVertexStream );
				_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iVertexStream ) );
			}
		}

		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::ShareExternal )
		{
			_indexBufferState.gpuBuffer = pSharedStorage.getIndexBuffer();
			_vertexStreamBindingMask.set( gpuapi::E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		}
	}

	void GeometryStorage::initializeVertexStreamState()
	{
		gpuapi::IAVertexStreamDefinition vertexStreamDefinition;
		vertexStreamDefinition.activeBindingsMask = _vertexStreamBindingMask;

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( const auto & vertexBuffer = _vertexBufferStateArray[iVertexStream].gpuBuffer )
			{
				auto & vertexBufferReference = vertexStreamDefinition.vertexBufferReferences[iVertexStream];
				vertexBufferReference.sourceBuffer = vertexBuffer;
				vertexBufferReference.relativeOffset = 0;
				vertexBufferReference.vertexStride = mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
			}
		}

		if( const auto & indexBuffer = _indexBufferState.gpuBuffer )
		{
			auto & indexBufferReference = vertexStreamDefinition.indexBufferReference;
			indexBufferReference.sourceBuffer = indexBuffer;
			indexBufferReference.relativeOffset = 0;
			indexBufferReference.indexFormat = mDataFormat.indexDataFormat();
		}

		_gpaVertexStreamState = mCES.mGPUDevice->createIAVertexStreamImmutableState( vertexStreamDefinition );
	}

	GeometryReference * GeometryStorage::addGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		auto & geometryReference = _geometryRefList.emplace_back();
		geometryReference.storage = this;
		geometryReference.dataFormat = &mDataFormat;
		geometryReference.geometryIndex = numeric_cast<uint32>( _geometryRefList.size() - 1 );

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( mDataFormat.isVertexStreamActive( iVertexStream ) )
			{
				const auto & vertexStreamFormat = mDataFormat.vertexStream( iVertexStream );

				auto & vertexStreamDataRegion = geometryReference.dataReference.vertexStreamDataRegions[iVertexStream];
				vertexStreamDataRegion.elementSize = vertexStreamFormat.elementSizeInBytes;
				vertexStreamDataRegion.offsetInElementsNum = _currentAllocationState.verticesOffsetInElementsNum;
				vertexStreamDataRegion.sizeInElementsNum = pVertexElementsNum;
			}
		}

		if( pIndexElementsNum > 0 )
		{
			if( mDataFormat.isIndexedGeometry() )
			{
				auto & indexDataRegion = geometryReference.dataReference.indexDataRegion;
				indexDataRegion.elementSize = mDataFormat.indexElementSizeInBytes();
				indexDataRegion.offsetInElementsNum = _currentAllocationState.indicesOffsetInElementsNum;
				indexDataRegion.sizeInElementsNum = pIndexElementsNum;
			}
		}

		_currentAllocationState.indicesOffsetInElementsNum += pIndexElementsNum;
		_currentAllocationState.verticesOffsetInElementsNum += pVertexElementsNum;

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

			if( pSharedStorage.mStorageMetrics.indexDataCapacityInElementsNum < pCreateInfo.metrics.indexDataCapacityInElementsNum )
			{
				return false;
			}

			const auto sourceIndexSize = cxdefs::getIndexDataFormatByteSize( pSharedStorage.mDataFormat.indexDataFormat() );
			const auto targetIndexSize = cxdefs::getIndexDataFormatByteSize( pCreateInfo.dataFormat->indexDataFormat() );
			if( sourceIndexSize != targetIndexSize )
			{
				return false;
			}
		}

		bool vertexBufferShared = false;

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				vertexBufferShared = true;

				const auto vertexBuffer = pSharedStorage.getVertexBuffer( iVertexStream );
				if( !vertexBuffer )
				{
					return false;
				}

				const auto sourceVertexComponentSize = pSharedStorage.mDataFormat.vertexStreamElementSizeInBytes( iVertexStream );
				const auto targetVertexComponentSize = pCreateInfo.dataFormat->vertexStreamElementSizeInBytes( iVertexStream );
				if( sourceVertexComponentSize != targetVertexComponentSize )
				{
					return false;
				}
			}
		}

		if( vertexBufferShared && ( pSharedStorage.mStorageMetrics.vertexDataCapacityInElementsNum < pCreateInfo.metrics.vertexDataCapacityInElementsNum ) )
		{
			return false;
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
