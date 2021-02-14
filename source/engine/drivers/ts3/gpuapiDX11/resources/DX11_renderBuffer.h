
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_RENDER_BUFFER_H__
#define __TS3DRIVER_GPUAPI_DX11_RENDER_BUFFER_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3::gpuapi
{

	class TS3GX_DX11_CLASS DX11RenderBuffer : public RenderBuffer
	{
	public:
		DXGI_FORMAT const mDXGITextureFormat;
		ComPtr<ID3D11Device1> const mD3D11Device1;
		ComPtr<ID3D11Texture2D> const mD3D11Texture2D;

	public:
		DX11RenderBuffer( DX11GPUDevice & pDX11GPUDevice,
		                  ERenderBufferType pRenderBufferType,
		                  const RenderBufferLayout & pRenderBufferLayout,
		                  DXGI_FORMAT pDXGITextureFormat,
		                  ComPtr<ID3D11Texture2D> pD3D11Texture2D );

		virtual ~DX11RenderBuffer();

		virtual bool isNull() const override;

		static Handle<DX11RenderBuffer> create( DX11GPUDevice & pDX11GPUDevice, const RenderBufferCreateInfo & pCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_DX11_RENDER_BUFFER_H__
