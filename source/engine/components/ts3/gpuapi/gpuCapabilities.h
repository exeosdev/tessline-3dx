
#pragma once

#ifndef __TS3_GPUAPI_GPU_CAPABILITIES_H__
#define __TS3_GPUAPI_GPU_CAPABILITIES_H__

#include "prerequisites.h"

namespace ts3::gpuapi
{

	enum EGPUDeviceFeatureFlags : uint32
	{
		E_GRAPHICS_DEVICE_FEATURE_FLAG_MULTI_THREADED_COMMAND_EXECUTION_BIT = 0x1,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_MULTI_THREADED_RESOURCE_CREATION_BIT = 0x2,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_RESOURCE_GPU_MEMORY_SPARSE_BINDING_BIT  = 0x4,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_SHADER_GEOMETRY_STAGE_BIT           = 0x10,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_SHADER_TESSELATION_STAGE_BIT        = 0x20,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_TEXTURE_ANISOTROPY_FILTER_BIT       = 0x40,
		E_GRAPHICS_DEVICE_FEATURE_FLAG_VIEWPORT_ARRAY_BIT                  = 0x80,
	};

	enum class EGraphicsCapFeatureID : std::underlying_type_t<EGPUDeviceFeatureFlags>
	{
		MultiThreadedCommandExecution,
		MultiThreadedResourceCreation,
		ResourceMemorySparseBinding,
		ShaderGeometryStage,
		ShaderTesselationStage,
		TextureAnisotropyFilter,
		ViewportArray,
		Unknown
	};

}

#endif // __TS3_GPUAPI_GPU_CAPABILITIES_H__
