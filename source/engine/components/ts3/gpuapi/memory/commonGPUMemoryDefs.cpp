
#include "commonGPUMemoryDefs.h"

namespace ts3::gpuapi
{

	namespace memutil
	{

		bool checkMemoryMapAccess( EGPUMemoryMapMode pRequestedMapMode, Bitmask<EGPUMemoryFlags> pMemoryFlags )
		{
			auto mapRequestedAccessFlags = static_cast<uint32>( pRequestedMapMode ) & E_GPU_MEMORY_MAP_FLAG_ACCESS_READ_WRITE_BIT;
			return pMemoryFlags.isSet( mapRequestedAccessFlags );
		}

		StructuredResourceAlignedMemory alignStructuredResourceDataAuto(
				const void * pData,
				native_uint pElementSize,
				native_uint pElementsNum )
		{
			StructuredResourceAlignedMemory resourceMemory;
			resourceMemory.metrics = computeStructuredResourceAlignedMemoryMetrics( pElementSize, pElementsNum );
			resourceMemory.alignedBuffer = alignStructuredResourceData( pData, resourceMemory.metrics );

			return resourceMemory;
		}

		DynamicByteArray alignStructuredResourceData(
				const void * pData,
				native_uint pElementSize,
				native_uint pElementsNum,
				memory_align_t pAlignedStride )
		{
			StructuredResourceAlignedMemoryMetrics memoryMetrics;
			memoryMetrics.elementSize = pElementSize;
			memoryMetrics.elementSizeAligned = pAlignedStride;
			memoryMetrics.elementsNum = pElementsNum;
			memoryMetrics.storageSize = pElementsNum * pElementSize;
			memoryMetrics.storageSizeAligned = pElementsNum * pAlignedStride;

			return alignStructuredResourceData( pData, memoryMetrics );
		}

		DynamicByteArray alignStructuredResourceData(
				const void * pData,
				const StructuredResourceAlignedMemoryMetrics & pMetrics )
		{
			ts3DebugAssert( pMetrics.elementSizeAligned >= pMetrics.elementSize );

			const auto paddingSize = pMetrics.elementSizeAligned - pMetrics.elementSize;

			DynamicByteArray alignedData;
			alignedData.resize( pMetrics.storageSizeAligned );

			auto byteView = bindMemoryView( pData, pMetrics.storageSize );
			const auto * srcPtr = byteView.data();
			auto * targetPtr = alignedData.data();

			for( native_uint elementIndex = 0; elementIndex < pMetrics.elementsNum; ++elementIndex )
			{
				memCopyUnchecked( targetPtr, pMetrics.elementSizeAligned, srcPtr, pMetrics.elementSize );
				memFillUnchecked( targetPtr + pMetrics.elementSize, pMetrics.elementSizeAligned, 0, paddingSize );

				srcPtr += pMetrics.elementSize;
				targetPtr += pMetrics.storageSizeAligned;
			}

			return alignedData;
		}

		StructuredResourceAlignedMemoryMetrics computeStructuredResourceAlignedMemoryMetrics(
				gpu_memory_size_t pElementSize,
				gpu_memory_size_t pElementsNum )
		{
			StructuredResourceAlignedMemoryMetrics memoryMetrics;
			memoryMetrics.elementSize = pElementSize;
			memoryMetrics.elementsNum = pElementsNum;
			memoryMetrics.elementSizeAligned = computeStructuredResourceAlignedStride( pElementSize );
			memoryMetrics.storageSize = pElementSize * pElementsNum;
			memoryMetrics.storageSizeAligned = memoryMetrics.elementSizeAligned * pElementsNum;

			return memoryMetrics;
		}

		gpu_memory_size_t computeStructuredResourceAlignedStride( gpu_memory_size_t pElementSize )
		{
			return memGetAlignedValue( pElementSize, 16 );
		}

	}

}
