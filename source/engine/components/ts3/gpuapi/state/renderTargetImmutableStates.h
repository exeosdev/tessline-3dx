
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__
#define __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__

#include "graphicsPipelineImmutableState.h"
#include "renderPassCommon.h"

namespace ts3::gpuapi
{

	class RenderTargetBindingImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderTargetLayout const mRenderTargetLayout;

	public:
		RenderTargetBindingImmutableState( const RenderTargetBindingImmutableState & ) = delete;
		RenderTargetBindingImmutableState & operator=( const RenderTargetBindingImmutableState & ) = delete;

		RenderTargetBindingImmutableState(
				GPUDevice & pGPUDevice,
				const RenderTargetLayout & pRenderTargetLayout );

		virtual ~RenderTargetBindingImmutableState();
	};

	class RenderPassConfigurationImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderPassConfigurationImmutableState( const RenderPassConfigurationImmutableState & ) = delete;
		RenderPassConfigurationImmutableState & operator=( const RenderPassConfigurationImmutableState & ) = delete;

		RenderPassConfigurationImmutableState( GPUDevice & pGPUDevice );
		virtual ~RenderPassConfigurationImmutableState();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__
