
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__

#include "graphicsPipelineImmutableState.h"
#include "graphicsShaderState.h"

namespace ts3::gpuapi
{

	struct GraphicsShaderLinkageCommonProperties
	{
		struct ActiveStageInfo
		{};
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

		GraphicsShaderLinkageImmutableState(
				GPUDevice & pGPUDevice,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties );

		virtual ~GraphicsShaderLinkageImmutableState();

		TS3_ATTR_NO_DISCARD bool isStageActive( uint32 pStageIndex ) const noexcept
		{
			return mCommonProperties.activeStagesMask.isSet( cxdefs::makeShaderStageBit( pStageIndex ) );
		}

		TS3_ATTR_NO_DISCARD bool isStageActive( EShaderType pShaderType ) const noexcept
		{
			return mCommonProperties.activeStagesMask.isSet( cxdefs::getShaderStageBit( pShaderType ) );
		}

		TS3_ATTR_NO_DISCARD uint32 getActiveStagesNum() const noexcept
		{
			return mCommonProperties.activeStagesNum;
		}
	};

	/// @brief
	class GraphicsShaderImmutableStateSeparable : public GraphicsShaderLinkageImmutableState
	{
	public:
		GraphicsShaderSet const mShaderSet;

	public:
		GraphicsShaderImmutableStateSeparable( const GraphicsShaderImmutableStateSeparable & ) = delete;
		GraphicsShaderImmutableStateSeparable & operator=( const GraphicsShaderImmutableStateSeparable & ) = delete;

		GraphicsShaderImmutableStateSeparable(
				GPUDevice & pGPUDevice,
				const GraphicsShaderSet & pShaderSet,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties );

		virtual ~GraphicsShaderImmutableStateSeparable();
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD GraphicsShaderLinkageCommonProperties getGraphicsShaderLinkageCommonPropertiesForShaderSet(
				const GraphicsShaderSet & pShaderSet ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
