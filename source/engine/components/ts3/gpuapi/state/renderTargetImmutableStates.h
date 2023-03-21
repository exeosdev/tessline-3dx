
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

		TS3_ATTR_NO_DISCARD static const RenderTargetBindingImmutableState & getDynamicOverrideState();
	};

	class RenderPassConfigurationImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderPassConfigurationImmutableState( const RenderPassConfigurationImmutableState & ) = delete;
		RenderPassConfigurationImmutableState & operator=( const RenderPassConfigurationImmutableState & ) = delete;

		RenderPassConfigurationImmutableState( GPUDevice & pGPUDevice );
		virtual ~RenderPassConfigurationImmutableState();
	};

	class RenderPassConfigurationImmutableStateDefault : public RenderPassConfigurationImmutableState
	{
	public:
		RenderPassConfiguration const mRenderPassConfiguration;

	public:
		RenderPassConfigurationImmutableStateDefault(
				GPUDevice & pGPUDevice,
				const RenderPassConfiguration & pRenderPassConfiguration );

		virtual ~RenderPassConfigurationImmutableStateDefault();

		static GpaHandle<RenderPassConfigurationImmutableStateDefault> createInstance(
				GPUDevice & pGPUDevice,
				const RenderPassConfiguration & pConfiguration );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__
