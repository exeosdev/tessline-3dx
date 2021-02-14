
#ifndef __TS3_ENGINE_GEOMETRY_STORAGE_H__
#define __TS3_ENGINE_GEOMETRY_STORAGE_H__

#include "geometryCommon.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	struct GPUBufferRef
	{
		gpuapi::GPUBufferHandle buffer = nullptr;
		gpuapi::MemoryRegion usedRegion = { 0, gpuapi::cxMemorySizeMax };

		explicit operator bool() const
		{
			return buffer && !usedRegion.empty();
		}
	};

	class GeometryStorage
	{
	public:
		GeometryDataType const mGeometryDataType;
		GPUBufferRef const mGPUBufferRef;

		bool isBufferMappable() const;

	private:
		GPUBufferRef _bufferRef;
	};

}

#endif // __TS3_ENGINE_GEOMETRY_STORAGE_H__
