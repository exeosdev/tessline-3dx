
#pragma once

#ifndef __TS3_GPUAPI_GPU_CAPABILITIES_H__
#define __TS3_GPUAPI_GPU_CAPABILITIES_H__

#include "prerequisites.h"

namespace ts3::gpuapi
{

	enum EGPUDeviceFeatureFlags : uint32
	{
		E_GPU_DEVICE_FEATURE_FLAG_MULTI_THREADED_COMMAND_EXECUTION_BIT   = 0x0001,
		E_GPU_DEVICE_FEATURE_FLAG_MULTI_THREADED_RESOURCE_CREATION_BIT   = 0x0002,

		E_GPU_DEVICE_FEATURE_FLAG_RESOURCE_GPU_MEMORY_SPARSE_BINDING_BIT = 0x0010,
		E_GPU_DEVICE_FEATURE_FLAG_TEXTURE_ANISOTROPY_FILTER_BIT          = 0x0020,
		E_GPU_DEVICE_FEATURE_FLAG_VIEWPORT_ARRAY_BIT                     = 0x0040,

		E_GPU_DEVICE_FEATURE_FLAG_SHADER_V1_STAGE_GEOMETRY_BIT           = 0x010000,
		E_GPU_DEVICE_FEATURE_FLAG_SHADER_V1_STAGE_TESSELATION_BIT        = 0x020000,
		E_GPU_DEVICE_FEATURE_FLAG_SHADER_V2_SUPPORT_BIT                  = 0x080000,
	};

	enum class EGraphicsCapFeatureID : std::underlying_type_t<EGPUDeviceFeatureFlags>
	{
		MultiThreadedCommandExecution,
		MultiThreadedResourceCreation,
		ResourceMemorySparseBinding,
		ShaderV1StageGeometry,
		ShaderV1StageTesselation,
		ShaderV2Support,
		TextureAnisotropyFilter,
		ViewportArray,
		Unknown
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_CAPABILITIES_H__
