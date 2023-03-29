
#include "geometryStorage.h"
#include "../coreEngineState.h"
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
		return ( pIndex < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM ) ? _vertexBufferStateArray[pIndex].gpuBuffer : nullptr;
	}

	GeometryReference * GeometryStorage::addIndexedGeometry( uint32 pIndicesNum, uint32 pVerticesNum )
	{
		if( ( pIndicesNum == 0 ) || ( pVerticesNum == 0 ) || !_indexBufferState.gpuBuffer )
		{
			return nullptr;
		}

		return addGeometry( pIndicesNum, pVerticesNum );
	}

	GeometryReference * GeometryStorage::addNonIndexedGeometry( uint32 pVerticesNum )
	{
		if( pVerticesNum == 0 )
		{
			return nullptr;
		}

		return addGeometry( 0, pVerticesNum );
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
		for( uint16 iStream = 0; iStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iStream )
		{
			const auto vertexComponentSize = pCreateInfo.geometryFormat.vertexComponentSizeArray[iStream];
			if( vertexComponentSize > 0 )
			{
				if( pCreateInfo.vertexBufferDescArray[iStream].allocationMode == EGeometryBufferAllocationMode::AllocLocal )
				{
					const auto bufferUsagePolicy = resolveGeometryBufferUsagePolicy(
							pCreateInfo.vertexBufferDescArray[iStream].bufferUsagePolicy,
							pCreateInfo.commonBufferUsagePolicy );

					_vertexBufferStateArray[iStream].gpuBuffer = createVertexBuffer(
							mCES,
							vertexComponentSize * pCreateInfo.metrics.vertexDataCapacityInElementsNum,
							bufferUsagePolicy );

					_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iStream ) );
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
		for( uint16 iStream = 0; iStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iStream )
		{
			if( pCreateInfo.vertexBufferDescArray[iStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				_vertexBufferStateArray[iStream].gpuBuffer = pSharedStorage.getVertexBuffer( iStream );
				_vertexStreamBindingMask.set( gpuapi::cxdefs::makeIAVertexBufferFlag( iStream ) );
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

		for( uint16 iStream = 0; iStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iStream )
		{
			if( const auto & vertexBuffer = _vertexBufferStateArray[iStream].gpuBuffer )
			{
				auto & vertexBufferReference = vertexStreamDefinition.vertexBufferReferences[iStream];
				vertexBufferReference.sourceBuffer = vertexBuffer;
				vertexBufferReference.relativeOffset = 0;
				vertexBufferReference.vertexStride = mGeometryFormatInfo.vertexComponentSizeArray[iStream];
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

	GeometryReference * GeometryStorage::addGeometry( uint32 pIndicesNum, uint32 pVerticesNum )
	{
		auto & geometryReference = _geometryRefList.emplace_back();
		geometryReference.geometryStorage = this;
		geometryReference.formatInfo = &mGeometryFormatInfo;
		geometryReference.geometryIndex = numeric_cast<uint32>( _geometryRefList.size() - 1 );

		for( uint16 iStream = 0; iStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iStream )
		{
			const auto vertexComponentSize = mGeometryFormatInfo.vertexComponentSizeArray[iStream];
			if( vertexComponentSize > 0 )
			{
				auto & dataRegion = geometryReference.vertexComponentDataRegionArray[iStream];
				dataRegion.offsetInElementsNum = _currentAllocationState.verticesOffsetInElementsNum;
				dataRegion.sizeInElementsNum = pVerticesNum;
			}
		}

		if( pIndicesNum > 0 )
		{
			const auto indexComponentSize = cxdefs::getIndexDataFormatByteSize( mGeometryFormatInfo.indexDataFormat );
			if( indexComponentSize > 0 )
			{
				auto & dataRegion = geometryReference.indexDataRegion;
				dataRegion.offsetInElementsNum = _currentAllocationState.indicesOffsetInElementsNum;
				dataRegion.sizeInElementsNum = pIndicesNum;
			}
		}
		else
		{
			auto & dataRegion = geometryReference.indexDataRegion;
			dataRegion.offsetInElementsNum = 0;
			dataRegion.sizeInElementsNum = 0;
		}

		_currentAllocationState.indicesOffsetInElementsNum += pIndicesNum;
		_currentAllocationState.verticesOffsetInElementsNum += pVerticesNum;

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

		for( uint32 iVertexStream = 0; iVertexStream < gpa::MAX_GEOMETRY_SEPARATE_VERTEX_COMPONENTS_NUM; ++iVertexStream )
		{
			if( pCreateInfo.vertexBufferDescArray[iVertexStream].allocationMode == EGeometryBufferAllocationMode::ShareExternal )
			{
				vertexBufferShared = true;

				const auto vertexBuffer = pSharedStorage.getVertexBuffer( iVertexStream );
				if( !vertexBuffer )
				{
					return false;
				}

				const auto sourceVertexComponentSize = pSharedStorage.mGeometryFormatInfo.vertexComponentSizeArray[iVertexStream];
				const auto targetVertexComponentSize = pCreateInfo.geometryFormat.vertexComponentSizeArray[iVertexStream];
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
