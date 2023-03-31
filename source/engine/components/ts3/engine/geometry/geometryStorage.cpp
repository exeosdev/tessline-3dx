
#include "geometryStorage.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources//gpuBuffer.h>

namespace ts3
{

	GeometryStorage::GeometryStorage(
			const CoreEngineState & pCES,
			const GeometryDataFormatInfo & pGeometryFormatInfo,
			const GeometryStorageMetrics & pStorageMetrics )
	: CoreEngineObject( pCES )
	, mGeometryFormatInfo( pGeometryFormatInfo )
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
			const GeometryStorageCreateInfo & pCreateInfo )
	{
		auto geometryStorage = std::make_unique<GeometryStorage>(
				pCES,
				pCreateInfo.geometryFormat,
				pCreateInfo.metrics );

		geometryStorage->createStorageGPUBuffers( pCreateInfo );

		return geometryStorage;
	}

	std::unique_ptr<GeometryStorage> GeometryStorage::createStorage(
			const CoreEngineState & pCES,
			const GeometryStorageCreateInfo & pCreateInfo,
			const GeometryStorage & pSharedStorage )
	{
		if( !validateSharedBuffersConfiguration( pCreateInfo, pSharedStorage ) )
		{
			return nullptr;
		}

		auto geometryStorage = std::make_unique<GeometryStorage>(
				pCES,
				pCreateInfo.geometryFormat,
				pCreateInfo.metrics );

		geometryStorage->createStorageGPUBuffers( pCreateInfo );
		geometryStorage->bindSharedGPUBuffers( pCreateInfo, pSharedStorage );

		return geometryStorage;
	}

	void GeometryStorage::createStorageGPUBuffers( const GeometryStorageCreateInfo & pCreateInfo )
	{
		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			const auto vertexStreamElementSize = pCreateInfo.geometryFormat.vertexStreamElementSizeArray[iVertexStream];
			if( vertexStreamElementSize > 0 )
			{
				if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::AllocLocal )
				{
					const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
							pCreateInfo.vertexBufferDescArray[iVertexStream].bufferUsagePolicy,
							pCreateInfo.commonBufferUsagePolicy );

					_vertexBufferStateArray[iVertexStream].gpuBuffer = createVertexBuffer(
							mCES,
							vertexStreamElementSize * pCreateInfo.metrics.vertexDataCapacityInElementsNum,
							bufferUsagePolicy );

					_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iVertexStream ) );
				}
			}
		}

		if( pCreateInfo.indexBufferDesc.allocationMode == EGeometryBufferAllocationMode::AllocLocal )
		{
			const auto indexComponentSize = pCreateInfo.geometryFormat.indexElementByteSize();
			if( indexComponentSize > 0 )
			{
				const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
						pCreateInfo.indexBufferDesc.bufferUsagePolicy,
						pCreateInfo.commonBufferUsagePolicy );

				_indexBufferState.gpuBuffer = createIndexBuffer(
						mCES,
						indexComponentSize * pCreateInfo.metrics.indexDataCapacityInElementsNum,
						bufferUsagePolicy );

				_vertexStreamBindingMask.set( gpuapi::E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
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
				vertexBufferReference.vertexStride = mGeometryFormatInfo.vertexStreamElementSizeArray[iVertexStream];
			}
		}

		if( const auto & indexBuffer = _indexBufferState.gpuBuffer )
		{
			auto & vertexBufferReference = vertexStreamDefinition.indexBufferReference;
			vertexBufferReference.sourceBuffer = indexBuffer;
			vertexBufferReference.relativeOffset = 0;
			vertexBufferReference.indexFormat = mGeometryFormatInfo.indexDataFormat;
		}

		_gpaVertexStreamState = mCES.mGPUDevice->createIAVertexStreamImmutableState( vertexStreamDefinition );
	}

	GeometryReference * GeometryStorage::addGeometry( uint32 pVertexElementsNum, uint32 pIndexElementsNum )
	{
		auto & geometryReference = _geometryRefList.emplace_back();
		geometryReference.geometryStorage = this;
		geometryReference.formatInfo = &mGeometryFormatInfo;
		geometryReference.geometryIndex = numeric_cast<uint32>( _geometryRefList.size() - 1 );

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_VERTEX_STREAMS_NUM; ++iVertexStream )
		{
			const auto vertexStreamElementSize = mGeometryFormatInfo.vertexStreamElementSizeArray[iVertexStream];
			if( vertexStreamElementSize > 0 )
			{
				auto & dataRegion = geometryReference.vertexStreamDataRegionArray[iVertexStream];
				dataRegion.offsetInElementsNum = _currentAllocationState.verticesOffsetInElementsNum;
				dataRegion.sizeInElementsNum = pVertexElementsNum;
			}
		}

		if( pIndexElementsNum > 0 )
		{
			const auto indexComponentSize = cxdefs::getIndexDataFormatByteSize( mGeometryFormatInfo.indexDataFormat );
			if( indexComponentSize > 0 )
			{
				auto & dataRegion = geometryReference.indexDataRegion;
				dataRegion.offsetInElementsNum = _currentAllocationState.indicesOffsetInElementsNum;
				dataRegion.sizeInElementsNum = pIndexElementsNum;
			}
		}
		else
		{
			auto & dataRegion = geometryReference.indexDataRegion;
			dataRegion.offsetInElementsNum = 0;
			dataRegion.sizeInElementsNum = 0;
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

			const auto sourceIndexSize = cxdefs::getIndexDataFormatByteSize( pSharedStorage.mGeometryFormatInfo.indexDataFormat );
			const auto targetIndexSize = cxdefs::getIndexDataFormatByteSize( pCreateInfo.geometryFormat.indexDataFormat );
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

				const auto sourceVertexComponentSize = pSharedStorage.mGeometryFormatInfo.vertexStreamElementSizeArray[iVertexStream];
				const auto targetVertexComponentSize = pCreateInfo.geometryFormat.vertexStreamElementSizeArray[iVertexStream];
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
