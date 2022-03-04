
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_STATE_OBJECT_H__

#include "DX11_pipelineStateDesc.h"
#include <ts3/gpuapi/state/renderTargetStateObject.h>

namespace ts3
{
namespace gpuapi
{

	class DX11RenderBuffer;
	class DX11Texture;

	struct DX11RenderTargetResourceState
	{
		ComPtr<ID3D11View>       colorAttachmentViewRef[GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		ID3D11RenderTargetView * colorAttachmentRTVArray[GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM];
		ComPtr<ID3D11View>       depthStencilAttachmentViewRef;
		ID3D11DepthStencilView * depthStencilAttachmentDSV;
	};

	class DX11RenderTargetStateObject : public RenderTargetStateObject
	{
	public:
		DX11RenderTargetResourceState const mDX11RTResourceState;

		DX11RenderTargetStateObject( DX11GPUDevice & pGPUDevice,
		                             const RenderTargetLayout & pRTLayout,
		                             const RenderTargetResourceBinding & pRTResourceBinding,
		                             DX11RenderTargetResourceState pDX11RTResourceState );

		virtual ~DX11RenderTargetStateObject();

		static Handle<DX11RenderTargetStateObject> create( DX11GPUDevice & pGPUDevice,
		                                                   const RenderTargetStateObjectCreateInfo & pCreateInfo );

	private:
		static ComPtr<ID3D11RenderTargetView> createRTVForRenderBuffer( DX11RenderBuffer & pDX11RenderBuffer,
		                                                                const RenderTargetAttachmentResourceBinding & pRTBinding );

		static ComPtr<ID3D11RenderTargetView> createRTVForTexture( DX11Texture & pDX11Texture,
		                                                           const RenderTargetAttachmentResourceBinding & pRTBinding );

		static ComPtr<ID3D11DepthStencilView> createDSVForRenderBuffer( DX11RenderBuffer & pDX11RenderBuffer,
		                                                                const RenderTargetAttachmentResourceBinding & pRTBinding );

		static ComPtr<ID3D11DepthStencilView> createDSVForTexture( DX11Texture & pDX11Texture,
		                                                           const RenderTargetAttachmentResourceBinding & pRTBinding );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_RENDER_TARGET_STATE_OBJECT_H__
