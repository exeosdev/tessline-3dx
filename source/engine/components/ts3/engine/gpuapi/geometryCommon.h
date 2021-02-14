
#ifndef __TS3_ENGINE_COMMON_RENDERER_DEFS_H__
#define __TS3_ENGINE_COMMON_RENDERER_DEFS_H__

#include "commonGpuAPIDefs.h"
#include <ts3/gpuapi/resources/gpuBufferCommon.h>

namespace ts3
{

	enum class GeometryDataType : enum_default_value_t
	{
		VertexData = static_cast<enum_default_value_t>( gpuapi::EGPUBufferTarget::VertexBuffer ),
		IndexData = static_cast<enum_default_value_t>( gpuapi::EGPUBufferTarget::IndexBuffer ),
	};

}

#endif // __TS3_ENGINE_COMMON_RENDERER_DEFS_H__

