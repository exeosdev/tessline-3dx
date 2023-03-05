
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__

#include "../graphicsPipelineImmutableState.h"
#include "../graphicsShaderState.h"

namespace ts3::gpuapi
{

	struct GraphicsShaderLinkageCommonProperties
	{
		Bitmask<EShaderStageFlags> activeStagesMask;
		uint32 activeStagesNum;
	};

	/// @brief
	class GraphicsShaderLinkageImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		GraphicsShaderLinkageCommonProperties const mCommonProperties;

	public:
		GraphicsShaderLinkageImmutableState( const GraphicsShaderLinkageImmutableState & ) = delete;
		GraphicsShaderLinkageImmutableState & operator=( const GraphicsShaderLinkageImmutableState & ) = delete;

		GraphicsShaderLinkageImmutableState( GPUDevice & pGPUDevice, const GraphicsShaderLinkageCommonProperties & pCommonProperties );
		virtual ~GraphicsShaderLinkageImmutableState() = default;

		TS3_ATTR_NO_DISCARD bool isStageActive( uint32 pStageIndex ) const;
		TS3_ATTR_NO_DISCARD bool isStageActive( EShaderType pShaderType ) const;
		TS3_ATTR_NO_DISCARD uint32 getActiveStagesNum() const;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
