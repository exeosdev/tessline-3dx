
#include "gpuBufferCommon.h"
#include "../resources/gpuBuffer.h"

namespace ts3::gpuapi
{

	namespace rcutil
	{

		gpu_memory_size_t queryGPUBufferByteSize( GPUBufferHandle pGPUBuffer )
		{
			return pGPUBuffer->mBufferProperties.byteSize;
		}

		bool checkGPUBufferRegion( GPUBufferHandle pGPUBuffer, gpu_memory_size_t pOffset, gpu_memory_size_t pSize )
		{
			const auto bufferSize = rcutil::queryGPUBufferByteSize( pGPUBuffer );
			return ( pSize > 0 ) && ( pOffset < bufferSize ) && ( pSize <= ( bufferSize - pOffset ) );
		}

		bool checkGPUBufferRegion( GPUBufferHandle pGPUBuffer, const GPUMemoryRegion & pRegion )
		{
			return checkGPUBufferRegion( pGPUBuffer, pRegion.offset, pRegion.size );
		}

		GPUMemoryRegion validateGPUBufferRegion( GPUBufferHandle pGPUBuffer, gpu_memory_size_t pOffset, gpu_memory_size_t pSize )
		{
			const auto bufferSize = rcutil::queryGPUBufferByteSize( pGPUBuffer );

			GPUMemoryRegion validRegion{ 0, 0 };
			validRegion.offset = getMinOf( pOffset, bufferSize );
			validRegion.size = getMinOf( pSize, bufferSize - pOffset );

			return validRegion;
		}

		GPUMemoryRegion validateGPUBufferRegion( GPUBufferHandle pGPUBuffer, const GPUMemoryRegion & pRegion )
		{
			return validateGPUBufferRegion( pGPUBuffer, pRegion.offset, pRegion.size );
		}

	}

}
