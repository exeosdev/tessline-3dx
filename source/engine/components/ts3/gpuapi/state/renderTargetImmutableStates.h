
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

		RenderTargetBindingImmutableState( GPUDevice & pGPUDevice, const RenderTargetLayout & pRenderTargetLayout );
		virtual ~RenderTargetBindingImmutableState();
	};

	class RenderPassImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderPassImmutableState( const RenderPassImmutableState & ) = delete;
		RenderPassImmutableState & operator=( const RenderPassImmutableState & ) = delete;

		RenderPassImmutableState( GPUDevice & pGPUDevice );
		virtual ~RenderPassImmutableState();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__
