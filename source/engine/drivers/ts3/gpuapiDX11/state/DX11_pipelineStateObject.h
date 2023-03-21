
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__

#include "../DX11_gpuDevice.h"
#include <ts3/gpuapi/state/separablePipelineState.h>

namespace ts3::gpuapi
{

	class DX11GraphicsPipelineStateObject : public GraphicsPipelineStateObjectSeparableShader
	{
	public:
		DX11GraphicsPipelineStateObject(
				DX11GPUDevice & pGPUDevice,
				RenderTargetLayout pRenderTargetLayout,
				ShaderInputSignature pShaderInputSignature,
				const SeparablePSOStateSet & pPSOImmutableStates,
				const GraphicsShaderSet & pSeparableShaders );

		virtual ~DX11GraphicsPipelineStateObject();

		TS3_ATTR_NO_DISCARD const DX11BlendImmutableState & getBlendState() const noexcept;

		TS3_ATTR_NO_DISCARD const DX11DepthStencilImmutableState & getDepthStencilState() const noexcept;

		TS3_ATTR_NO_DISCARD const DX11RasterizerImmutableState & getRasterizerState() const noexcept;

		TS3_ATTR_NO_DISCARD const DX11GraphicsShaderLinkageImmutableState & getGraphicsShaderLinkageState() const noexcept;

		TS3_ATTR_NO_DISCARD const DX11IAInputLayoutImmutableState & getIAInputLayoutState() const noexcept;

		static GpaHandle<DX11GraphicsPipelineStateObject> create(
				DX11GPUDevice & pGPUDevice,
				const GraphicsPipelineStateObjectCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_PIPELINE_STATE_OBJECT_H__
