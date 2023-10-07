
#pragma once

#ifndef __TS3_ENGINE_SHADOW_RENDERING_CONTEXT_H__
#define __TS3_ENGINE_SHADOW_RENDERING_CONTEXT_H__

#include "commonRendererDefs.h"

namespace ts3
{

	struct ContextResourcesBindingState
	{
		gpuapi::GPUBufferHandle constantBuffers[cxdefs::GPA_SHADER_BINDING_METRIC_CB_MAX_NUM];

		TextureUnitBinding textureUnits[cxdefs::GPA_SHADER_BINDING_METRIC_TEX_MAX_NUM];
	};

	class RenderingContext
	{
	public:
		gpuapi::GPUDevice & mGPUDevice;
		gpuapi::CommandContext & mGPUCommandContext;

	public:
		RenderingContext( gpuapi::CommandContext & pGPUCommandContext );
		~RenderingContext();

		void createConstantBuffer( native_uint pIndex, memory_size_t pBufferSize, bool pMapped = false );

		void bindConstantBuffer( native_uint pIndex, gpuapi::GPUBufferHandle pConstantBuffer );

		void bindTexture( native_uint pIndex, gpuapi::TextureHandle pTexture, gpuapi::SamplerHandle pSampler );

		gpuapi::GPUBufferHandle getConstantBuffer( native_uint pIndex ) const noexcept;

	private:
		ContextResourcesBindingState _bindingState;
	};

} // namespace ts3

#endif // __TS3_ENGINE_SHADOW_RENDERING_CONTEXT_H__
