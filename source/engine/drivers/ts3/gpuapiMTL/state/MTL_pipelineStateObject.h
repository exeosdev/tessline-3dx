
#pragma once

#ifndef __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_OBJECT_H__

#include "../MTL_prerequisites.h"
#include <ts3/gpuapi/state/pipelineStateObject.h>

namespace ts3::gpuapi
{

	class MetalGraphicsPipelineStateObject : public GraphicsPipelineStateObject
	{
	public:
		id<MTLRenderPipelineState> const mMTLPipelineState;

	public:
		MetalGraphicsPipelineStateObject(
				MetalGPUDevice & pGPUDevice,
				RenderTargetLayout pRenderTargetLayout,
				ShaderInputSignature pShaderInputSignature,
				id<MTLRenderPipelineState> pMTLPipelineState );

		virtual ~MetalGraphicsPipelineStateObject();

		static GpaHandle<MetalGraphicsPipelineStateObject> create(
				MetalGPUDevice & pGPUDevice,
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_MTLCOMMON_PIPELINE_STATE_OBJECT_H__
