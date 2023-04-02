
#pragma once

#ifndef __TS3_ENGINE_COMMON_RENDERER_DEFS_H__
#define __TS3_ENGINE_COMMON_RENDERER_DEFS_H__

#include "../prerequisites.h"

#include <ts3/gpuapi/resources/textureCommon.h>
#include <ts3/gpuapi/state/commonGPUStateDefs.h>

namespace ts3
{

	ts3DeclareClassHandle( GpaSharedStateLibrary );
	ts3DeclareClassHandle( ShaderLibrary );
	ts3DeclareClassHandle( ShaderLoader );

	using GpaUniqueObjectID = gpuapi::UniqueGPUObjectID;
	using GpaUniqueObjectName = gpuapi::UniqueGPUObjectName;

	namespace cxdefs
	{

		constexpr uint32 GPA_SHADER_BINDING_METRIC_CB_MAX_NUM = gpuapi::gpm::RES_MAX_CONSTANT_BUFFERS_NUM;

		constexpr uint32 GPA_SHADER_BINDING_METRIC_TEX_MAX_NUM = gpuapi::gpm::RES_MAX_TEXTURE_UNITS_NUM;

	}

	struct TextureUnitBinding
	{
		gpuapi::TextureHandle texture;
		gpuapi::SamplerHandle sampler;
	};

} // namespace ts3

#endif // __TS3_ENGINE_COMMON_RENDERER_DEFS_H__
