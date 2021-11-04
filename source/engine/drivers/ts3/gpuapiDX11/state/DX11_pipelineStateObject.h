
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__

#include "DX11_pipelineStateDesc.h"
#include <ts3/gpuapi/state/pipelineStateObject.h>

namespace ts3
{
namespace gpuapi
{

	class DX11GraphicsPipelineStateObject : public SeparableGraphicsPipelineStateObject
	{
	public:
		DX11GraphicsPipelineStateObject( DX11GPUDevice & pGPUDevice,
		                                 RenderTargetLayout pRenderTargetLayout,
		                                 GraphicsShaderBinding pShaderBinding,
		                                 ShaderInputSignature pShaderInputSignature,
		                                 const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet );

		virtual ~DX11GraphicsPipelineStateObject();

		static Handle<DX11GraphicsPipelineStateObject> create( DX11GPUDevice & pGPUDevice,
		                                                       const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__
