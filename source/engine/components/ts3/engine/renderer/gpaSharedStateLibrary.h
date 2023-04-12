
#pragma once

#ifndef __TS3_ENGINE_GPA_SHARED_STATE_LIBRARY_H__
#define __TS3_ENGINE_GPA_SHARED_STATE_LIBRARY_H__

#include "commonRendererDefs.h"
#include <ts3/gpuapi/state/commonGPUStateDefs.h>
#include <ts3/gpuapi/resources/shaderCommon.h>
#include <unordered_map>

namespace ts3
{

	namespace cxdefs
	{

		constexpr GpaUniqueObjectID GPA_RESOURCE_ID_SHADER_PASSTHROUGH_IA_DEFAULT_VS { 0xFF007001 };
		constexpr GpaUniqueObjectID GPA_RESOURCE_ID_SHADER_PASSTHROUGH_IA_DEFAULT_PS { 0xFF007002 };

		constexpr GpaUniqueObjectID GPA_STATE_ID_BLEND_DEFAULT { 0xFF00BB01 };

		constexpr GpaUniqueObjectID GPA_STATE_ID_DEPTH_STENCIL_DEFAULT { 0xFF00DD01 };
		constexpr GpaUniqueObjectID GPA_STATE_ID_DEPTH_STENCIL_DEPTH_TEST_ENABLE { 0xFF00DD02 };

		constexpr GpaUniqueObjectID GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_BACK_CCW { 0xFF00CC01 };
		constexpr GpaUniqueObjectID GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_BACK_CW { 0xFF00CC02 };
		constexpr GpaUniqueObjectID GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_FRONT_CCW { 0xFF00CC03 };
		constexpr GpaUniqueObjectID GPA_STATE_ID_RASTERIZER_DEFAULT_CULL_FRONT_CW { 0xFF00CC04 };

		constexpr GpaUniqueObjectID GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT { 0xFF00AA01 };
		constexpr GpaUniqueObjectID GPA_STATE_ID_IA_INPUT_LAYOUT_DEFAULT_16B { 0xFF00AA02 };

		constexpr GpaUniqueObjectID GPA_STATE_ID_GRAPHICS_SHADER_LINKAGE_PASSTHROUGH_IA_DEFAULT { 0xFF00EE01 };

	}

	class GpaSharedStateLibrary : public CoreEngineObject
	{
	public:
		ShaderLibraryHandle const mShaderLibrary;

	public:
		explicit GpaSharedStateLibrary( const CoreEngineState & pCES, ShaderLibraryHandle pShaderLibrary = nullptr );
		~GpaSharedStateLibrary();

		TS3_ATTR_NO_DISCARD gpuapi::ShaderHandle getShader( GpaUniqueObjectID pShaderID ) const noexcept;
		TS3_ATTR_NO_DISCARD gpuapi::ShaderHandle getShader( const GpaUniqueObjectName & pShaderName ) const noexcept;

		void initialize();

	private:
		void createDefaultShaders();
		void initializeDefaultImmutableBlendStates();
		void initializeDefaultImmutableDepthStencilStates();
		void initializeDefaultImmutableRasterizerStates();
		void initializeDefaultImmutableInputLayoutStates();
		void initializeDefaultPipelineStateObjects();
	};

} // namespace ts3

#endif // __TS3_ENGINE_GPA_SHARED_STATE_LIBRARY_H__
