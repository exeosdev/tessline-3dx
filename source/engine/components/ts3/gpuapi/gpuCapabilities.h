
#pragma once

#ifndef __TS3_GPUAPI_GPU_CAPABILITIES_H__
#define __TS3_GPUAPI_GPU_CAPABILITIES_H__

#include "prerequisites.h"

namespace ts3::gpuapi
{

	enum EGPUDeviceFeatureFlags : uint32
	{
		E_GPU_DEVICE_FEATURE_FLAG_MULTI_THREADED_COMMAND_EXECUTION_BIT      = 1 << 0,
		E_GPU_DEVICE_FEATURE_FLAG_MULTI_THREADED_RESOURCE_CREATION_BIT      = 1 << 1,
		E_GPU_DEVICE_FEATURE_FLAG_RESOURCE_GPU_MEMORY_SPARSE_BINDING_BIT    = 1 << 2,
		E_GPU_DEVICE_FEATURE_FLAG_SAMPLER_CUSTOM_BORDER_COLOR_BIT           = 1 << 3,
		E_GPU_DEVICE_FEATURE_FLAG_SHADER_VERTEX_BASED_STAGE_GEOMETRY_BIT    = 1 << 6,
		E_GPU_DEVICE_FEATURE_FLAG_SHADER_VERTEX_BASED_STAGE_TESSELATION_BIT = 1 << 7,
		E_GPU_DEVICE_FEATURE_FLAG_SHADER_MESH_BASED_SUPPORT_BIT             = 1 << 8,
		E_GPU_DEVICE_FEATURE_FLAG_TEXTURE_ANISOTROPY_FILTER_BIT             = 1 << 4,
		E_GPU_DEVICE_FEATURE_FLAG_VIEWPORT_ARRAY_BIT                        = 1 << 5,
	};

	enum class EGraphicsCapFeatureID : std::underlying_type_t<EGPUDeviceFeatureFlags>
	{
		MultiThreadedCommandExecution,
		MultiThreadedResourceCreation,
		ResourceMemorySparseBinding,
		SamplerCustomBorderColor,
		ShaderVertexBasedStageGeometry,
		ShaderVertexBasedStageTesselation,
		ShaderMeshBasedSupport,
		TextureAnisotropyFilter,
		ViewportArray,
		Unknown
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GPU_CAPABILITIES_H__
