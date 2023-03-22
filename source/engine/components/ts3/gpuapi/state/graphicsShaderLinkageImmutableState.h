
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__

#include "graphicsPipelineImmutableState.h"
#include "graphicsShaderState.h"

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

		TS3_ATTR_NO_DISCARD virtual Shader * getShader( size_t pIndex ) const noexcept;

		TS3_ATTR_NO_DISCARD virtual Shader * getShader( EShaderType pShaderType ) const noexcept;
	};

	/// @brief
	class GraphicsShaderLinkageImmutableStateSeparable : public GraphicsShaderLinkageImmutableState
	{
	public:
		GraphicsShaderSet const mShaderSet;

	public:
		GraphicsShaderLinkageImmutableStateSeparable( const GraphicsShaderLinkageImmutableStateSeparable & ) = delete;
		GraphicsShaderLinkageImmutableStateSeparable & operator=( const GraphicsShaderLinkageImmutableStateSeparable & ) = delete;

		GraphicsShaderLinkageImmutableStateSeparable(
				GPUDevice & pGPUDevice,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties,
				const GraphicsShaderSet & pShaderSet );

		virtual ~GraphicsShaderLinkageImmutableStateSeparable();

		TS3_ATTR_NO_DISCARD virtual Shader * getShader( size_t pIndex ) const noexcept override final;

		TS3_ATTR_NO_DISCARD virtual Shader * getShader( EShaderType pShaderType ) const noexcept override final;
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD GraphicsShaderLinkageCommonProperties getGraphicsShaderLinkageCommonPropertiesForShaderSet(
				const GraphicsShaderSet & pShaderSet ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_SHADER_BINDING_IMMUTABLE_STATE_H__
