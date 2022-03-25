
#include "hwBufferManager.h"
#include <ts3/gpuapi/gpuDevice.h>
#include <ts3/gpuapi/resources/gpuBuffer.h>

namespace ts3
{

	ResultCode HWBufferManager::allocateGPUBufferExplicit( gpuapi_buffer_ref_id_t pGPUBufferRefID,
														   const gpuapi::GPUBufferCreateInfo & pGBCreateInfo )
	{
		return {};
	}

	HWVertexBufferHandle HWBufferManager::createVertexBuffer( const HWBufferCreateInfo & pHWBCreateInfo )
	{
		auto validatedHWBCreateInfo = pHWBCreateInfo;
		_validateBufferCreateInfo( EHWBufferType::HBTVertexBuffer, validatedHWBCreateInfo );

		auto gpuBuffer = _createGPUBuffer( CX_GPUAPI_BUFFER_REF_ID_AUTO, validatedHWBCreateInfo );
		if( !gpuBuffer )
		{
			ts3DebugOutput( "HWB: GPU buffer creation has failed" );
			return nullptr;
		}

		return nullptr;
	}

	HWVertexBufferHandle HWBufferManager::createVertexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
	                                                            const HWBufferCreateInfo & pHWBCreateInfo )
	{
		return nullptr;
	}

	HWIndexBufferHandle HWBufferManager::createIndexBuffer( const HWBufferCreateInfo & pHWBCreateInfo )
	{
		auto validatedHWBCreateInfo = pHWBCreateInfo;
		_validateBufferCreateInfo( EHWBufferType::HBTIndexBuffer, validatedHWBCreateInfo );

		auto gpuBuffer = _createGPUBuffer( CX_GPUAPI_BUFFER_REF_ID_AUTO, validatedHWBCreateInfo );
		if( !gpuBuffer )
		{
			ts3DebugOutput( "HWB: GPU buffer creation has failed" );
			return nullptr;
		}

		return nullptr;
	}

	HWIndexBufferHandle HWBufferManager::createIndexBufferEx( gpuapi_buffer_ref_id_t pGBUBufferRefID,
	                                                          const HWBufferCreateInfo & pHWBCreateInfo )
	{
		return nullptr;
	}

	GPUBufferUsageInfo HWBufferManager::getGPUBufferInfo( gpuapi_buffer_ref_id_t pGPUBufferRefID ) const
	{
		return {};
	}

	memory_align_t HWBufferManager::queryAlignmentRequirementsForBuffer( EHWBufferType pBufferType,
	                                                                     gpuapi::gpu_memory_size_t pBufferSize,
	                                                                     Bitmask<gpuapi::gpu_memory_flags_value_t> pMemoryFlags )
	{
		return CX_MEMORY_DEFAULT_ALIGNMENT;
	}

	gpuapi::GPUBufferHandle HWBufferManager::_createGPUBuffer( gpuapi_buffer_ref_id_t pGPUBufferRefID,
	                                                           const HWBufferCreateInfo & pHWBCreateInfo )
	{
		const auto requestedBufferSize = pHWBCreateInfo.metrics.uGeneric.bufferSize;
		if( requestedBufferSize == 0 )
		{
			return nullptr;
		}

		// All buffer-related flags at the engine level are combined into a single enum.
		// Extract MemoryFlags part using the dedicated utility function.
		const auto memoryFlags = ecGetHWBufferUsageGPUMemoryFlags( pHWBCreateInfo.flags );

		// Compute the alignment using the provided buffer specification.
		// If this is '0', everything is fine - it means default alignment for the current platform.
		const auto memoryAlignment = queryAlignmentRequirementsForBuffer( pHWBCreateInfo.baseType, requestedBufferSize, memoryFlags );

		gpuapi::GPUBufferCreateInfo gpuBufferCreateInfo;
		gpuBufferCreateInfo.bufferSize = requestedBufferSize;
		gpuBufferCreateInfo.initialTarget = static_cast<gpuapi::EGPUBufferTarget>( pHWBCreateInfo.baseType );
		gpuBufferCreateInfo.resourceFlags = ecGetHWBufferUsageGPUResourceFlags( pHWBCreateInfo.flags );
		gpuBufferCreateInfo.memoryFlags = memoryFlags;
		gpuBufferCreateInfo.memoryBaseAlignment = memoryAlignment;
		gpuBufferCreateInfo.initDataDesc = pHWBCreateInfo.initData;

		auto gpuBuffer = _gpuDevice->createGPUBuffer( gpuBufferCreateInfo );
		if( !gpuBuffer )
		{
			return nullptr;
		}

		if( pGPUBufferRefID == CX_GPUAPI_BUFFER_REF_ID_AUTO )
		{
			// If no explicit refID was specified, use the GPU buffer's address.
			pGPUBufferRefID = reinterpret_cast<gpuapi_buffer_ref_id_t>( gpuBuffer.get() );
		}
		else
		{
			auto existingBufferState = _gpuBufferMap.find( pGPUBufferRefID );
			if( existingBufferState != _gpuBufferMap.end() )
			{
				ts3DebugOutputFmt( "HWB: specified refID for GPUBuffer (0x%x) has been already used!", pGPUBufferRefID );
				ts3DebugInterrupt();
				return nullptr;
			}
		}

		GPUBufferState gpuBufferState;
		gpuBufferState.gpuBuffer = gpuBuffer;
		gpuBufferState.currentAllocOffset = 0;

		_gpuBufferMap[pGPUBufferRefID] = std::move( gpuBufferState );

		return gpuBuffer;
	}

	GPUBufferRef HWBufferManager::_reserveGPUBufferRegion( gpuapi_buffer_ref_id_t pGPUBufferRefID,
	                                                       gpuapi::gpu_memory_size_t pSize,
	                                                       memory_align_t pAlignment )
	{
		auto internalBufferStateIter = _gpuBufferMap.find( pGPUBufferRefID );
		if( internalBufferStateIter == _gpuBufferMap.end() )
		{
			return nullptr;
		}

		auto & gpuBufferState = internalBufferStateIter->second;

		const auto & bufferProperties = gpuBufferState.gpuBuffer->mBufferProperties;
		const auto & bufferMemoryInfo = gpuBufferState.gpuBuffer->mResourceMemory;

		// Total size of the source buffer, in bytes.
		const auto gpuBufferSize = bufferProperties.byteSize;

		if( gpuBufferSize < pSize )
		{
			return nullptr;
		}

		// GPUBuffer has some memory allocated for it, described as a heap subregion (an offset and a size).
		// Our internal 'currentAllocOffset' is a relative offset from the beginning of that heap subregion
		// (initially: 0). Compute the "actual" current heap offset by adding those two. We need to work with
		// the actual address to properly handle alignment: if base heap address was 4-byte aligned and there
		// was already an allocation of 204 bytes, for example, current heap offset meets 16 byte alignment
		// requirement, even though its value is 12.
		const auto baseHeapOffset = bufferMemoryInfo.sourceHeapRegion.offset + gpuBufferState.currentAllocOffset;

		// If a more strict alignment was requested, just use it.
		const auto allocAlignment = getMaxOf( pAlignment, bufferMemoryInfo.baseAlignment );

		// Align the offset properly so it meets the requirements. Note, that after aligning,
		// this offset can go beyond the memory range valid for this GPU buffer.
		const auto alignedHeapOffset = memGetAlignedValue( baseHeapOffset, allocAlignment );

		// The size of extra memory we need to allocate.
		const auto offsetDiff = alignedHeapOffset - baseHeapOffset;

		// Total allocation size
		const auto allocationSize = pSize + offsetDiff;

		if( allocationSize > gpuBufferState.availableMemorySize )
		{
			return nullptr;
		}

		GPUBufferRef gpuBufferRef;
		gpuBufferRef.buffer = gpuBufferState.gpuBuffer;
		gpuBufferRef.reservedRegion.offset = baseHeapOffset;
		gpuBufferRef.reservedRegion.size = allocationSize;
		gpuBufferRef.memoryRegion.offset = alignedHeapOffset;
		gpuBufferRef.memoryRegion.size = pSize;

		gpuBufferState.currentAllocOffset += allocationSize;
		gpuBufferState.availableMemorySize -= allocationSize;

		return gpuBufferRef;
	}

	void HWBufferManager::_validateBufferCreateInfo( EHWBufferType pBufferType, HWBufferCreateInfo & pHWBCreateInfo )
	{
		if( ( pHWBCreateInfo.baseType != EHWBufferType::HBTUnknown ) && ( pHWBCreateInfo.baseType != pBufferType ) )
		{
			ts3DebugOutput( "HWB: Invalid baseType specified for a buffer. Buffer class type will be used instead." );
			pHWBCreateInfo.baseType = pBufferType;
		}
	}

}
