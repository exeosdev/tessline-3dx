
#include "gpuBufferCommon.h"
#include "../resources/gpuBuffer.h"

namespace ts3::gpuapi
{

	namespace rcutil
	{

		EGPUBufferTarget getGPUBufferDefaultTargetFromBindFlags( Bitmask<resource_flags_value_t> pBindFlags )
		{
			const uint32 orderedBindMaskArray[] =
			{
				E_GPU_BUFFER_BIND_FLAG_CONSTANT_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_STREAM_OUTPUT_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_SHADER_INPUT_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_SHADER_UAV_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_INDIRECT_DISPATCH_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_INDIRECT_DRAW_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_TRANSFER_SOURCE_BUFFER_BIT,
				E_GPU_BUFFER_BIND_FLAG_TRANSFER_TARGET_BUFFER_BIT
			};

			for( auto bindMask : orderedBindMaskArray )
			{
				if( pBindFlags.isSet( bindMask ) )
				{
					return static_cast<EGPUBufferTarget>( bindMask );
				}
			}

			return EGPUBufferTarget::Unknown;
		}

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
