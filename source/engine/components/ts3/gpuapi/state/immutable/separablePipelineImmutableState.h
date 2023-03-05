
#pragma once

#ifndef __TS3_GPUAPI_SEPARABLE_PIPELINE_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_SEPARABLE_PIPELINE_IMMUTABLE_STATE_H__

#include "graphicsShaderLinkageImmutableState.h"

namespace ts3::gpuapi
{

	/// @brief
	class SeparableGraphicsShaderImmutableState : public GraphicsShaderLinkageImmutableState
	{
	public:
		GraphicsShaderSet const mShaderSet;

	public:
		SeparableGraphicsShaderImmutableState( const SeparableGraphicsShaderImmutableState & ) = delete;
		SeparableGraphicsShaderImmutableState & operator=( const SeparableGraphicsShaderImmutableState & ) = delete;

		SeparableGraphicsShaderImmutableState(
				GPUDevice & pGPUDevice,
				const GraphicsShaderSet & pShaderSet,
				const GraphicsShaderLinkageCommonProperties & pCommonProperties );

		virtual ~SeparableGraphicsShaderImmutableState();
	};

	/// @brief
	class SeparablePipelineImmutableStateFactory : public PipelineImmutableStateFactory
	{
	public:
		virtual GraphicsShaderLinkageImmutableStateHandle createGraphicsShaderLinkageState( const GraphicsShaderSet & pShaderSet ) override;
	};

	namespace smutil
	{

		TS3_GPUAPI_API_NO_DISCARD GraphicsShaderLinkageCommonProperties getGraphicsShaderLinkageCommonPropertiesForShaderSet(
				const GraphicsShaderSet & pShaderSet ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_SEPARABLE_PIPELINE_IMMUTABLE_STATE_H__
