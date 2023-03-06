
#pragma once

#ifndef __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__
#define __TS3_GPUAPI_RENDER_TARGET_IMMUTABLE_STATES_H__

#include "../graphicsPipelineImmutableState.h"
#include "../renderPassCommon.h"

namespace ts3::gpuapi
{

	class RenderTargetLayoutImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderTargetLayoutConfiguration const mLayoutConfiguration;
		
	public:
		RenderTargetLayoutImmutableState( const RenderTargetLayoutImmutableState & ) = delete;
		RenderTargetLayoutImmutableState & operator=( const RenderTargetLayoutImmutableState & ) = delete;

		RenderTargetLayoutImmutableState( GPUDevice & pGPUDevice, const RenderTargetLayoutConfiguration & pLayoutConfiguration );
		virtual ~RenderTargetLayoutImmutableState();
	};

	class RenderTargetBindingImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		RenderTargetBindingDefinition const mBindingDefinition;

	public:
		RenderTargetBindingImmutableState( const RenderTargetBindingImmutableState & ) = delete;
		RenderTargetBindingImmutableState & operator=( const RenderTargetBindingImmutableState & ) = delete;

		RenderTargetBindingImmutableState( GPUDevice & pGPUDevice, const RenderTargetBindingDefinition & pBindingDefinition );
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
