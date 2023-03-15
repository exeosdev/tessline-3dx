
#include "MTL_gpuBuffer.h"
#include "../MTL_commandList.h"
#include "../MTL_gpuDevice.h"

#include <Metal/MTLDevice.h>

namespace ts3::gpuapi
{

	MetalGPUBuffer::MetalGPUBuffer(
			MetalGPUDevice & pGPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const GPUBufferProperties & pBufferProperties,
			id<MTLBuffer> pMTLBuffer )
	: GPUBuffer( pGPUDevice, pResourceMemory, pBufferProperties )
	, mMTLDevice( [pMTLBuffer device] )
	, mMTLBuffer( pMTLBuffer )
	{}

	MetalGPUBuffer::~MetalGPUBuffer() = default;

	MetalGPUBufferHandle MetalGPUBuffer::create( MetalGPUDevice & pGPUDevice, const GPUBufferCreateInfo & pCreateInfo )
	{}

	bool MetalGPUBuffer::mapRegion( void *, const GPUMemoryRegion & pRegion, EGPUMemoryMapMode pMapMode )
	{
		auto * baseBufferPointer = [mMTLBuffer contents];
		if( !baseBufferPointer )
		{
			return false;
		}

		ResourceMappedMemory mappedMemoryInfo;
		mappedMemoryInfo.pointer = baseBufferPointer;
		mappedMemoryInfo.mappedRegion = pRegion;
		mappedMemoryInfo.memoryMapFlags = static_cast<EGPUMemoryMapFlags>( pMapMode );
		setMappedMemory( mappedMemoryInfo );

		return true;
	}

	void MetalGPUBuffer::unmap( void * )
	{
		if( const auto & mappedMemory = getMappedMemory() )
		{
			NSRange updateRange;
			updateRange.location = numeric_cast<NSUInteger>( mappedMemory.mappedRegion.offset );
			updateRange.length = numeric_cast<NSUInteger>( mappedMemory.mappedRegion.size );

			[mMTLBuffer didModifyRange:updateRange];

			resetMappedMemory();
		}
	}

	void MetalGPUBuffer::flushMappedRegion( void *, const GPUMemoryRegion & pRegion )
	{
		if( const auto & mappedMemory = getMappedMemory() )
		{
			NSRange updateRange;
			updateRange.location = numeric_cast<NSUInteger>( mappedMemory.mappedRegion.offset );
			updateRange.length = numeric_cast<NSUInteger>( mappedMemory.mappedRegion.size );

			[mMTLBuffer didModifyRange:updateRange];
		}
	}

	void MetalGPUBuffer::invalidateRegion( void * pCommandObject, const GPUMemoryRegion & pRegion )
	{
	}

	void MetalGPUBuffer::updateSubDataCopy( void * pCommandObject, GPUBuffer & pSrcBuffer, const GPUBufferSubDataCopyDesc & pCopyDesc )
	{
		auto * metalCommandList = reinterpret_cast<MetalCommandList *>( pCommandObject );
		if( !metalCommandList->checkCommandClassSupport( ECommandQueueClass::Transfer ) )
		{
			throw 0;
		}

		auto * sourceMTLBuffer = pSrcBuffer.queryInterface<MetalGPUBuffer>()->mMTLBuffer;

		id<MTLBlitCommandEncoder> blitCommandEncoder = [metalCommandList->mMTLCommandBuffer blitCommandEncoder];

		[blitCommandEncoder copyFromBuffer:sourceMTLBuffer
		                      sourceOffset:pCopyDesc.sourceBufferRegion.offset
		                          toBuffer:mMTLBuffer
		                 destinationOffset:pCopyDesc.targetBufferOffset
		                              size:pCopyDesc.sourceBufferRegion.size];

		[blitCommandEncoder endEncoding];

	}

	void MetalGPUBuffer::updateSubDataUpload( void * pCommandObject, const GPUBufferSubDataUploadDesc & pUploadDesc )
	{
		auto * metalCommandList = reinterpret_cast<MetalCommandList *>( pCommandObject );
		if( !metalCommandList->checkCommandClassSupport( ECommandQueueClass::Transfer ) )
		{
			throw 0;
		}

		id<MTLBuffer> uploadBuffer = [mMTLDevice newBufferWithBytes:pUploadDesc.inputDataDesc.pointer
									                         length:pUploadDesc.inputDataDesc.size
									                        options:MTLResourceStorageModePrivate | MTLResourceCPUCacheModeDefaultCache];

		id<MTLBlitCommandEncoder> blitCommandEncoder = [metalCommandList->mMTLCommandBuffer blitCommandEncoder];

		[blitCommandEncoder copyFromBuffer:uploadBuffer
		                      sourceOffset:0
		                          toBuffer:mMTLBuffer
		                 destinationOffset:pUploadDesc.bufferRegion.offset
		                              size:pUploadDesc.bufferRegion.size];

		[blitCommandEncoder endEncoding];
	}

	bool MetalGPUBuffer::validateMapRequest( const GPUMemoryRegion & pRegion, const EGPUMemoryMapMode & pMapMode )
	{
		return true;
	}

	id<MTLCommandBuffer> MetalGPUBuffer::getMTLCommandBuffer( void * pCommandObject )
	{
		return reinterpret_cast<MetalCommandList *>( pCommandObject )->mMTLCommandBuffer;
	}

}
