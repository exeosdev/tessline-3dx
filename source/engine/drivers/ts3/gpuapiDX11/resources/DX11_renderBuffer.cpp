
#include "DX11_renderBuffer.h"
#include <ts3/gpuapiDX11/DX11_coreAPIProxy.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/resources/DX11_texture.h>

namespace ts3::gpuapi
{

	DX11RenderBuffer::DX11RenderBuffer( DX11GPUDevice & pDX11GPUDevice,
	                                    ERenderBufferType pRenderBufferType,
	                                    const RenderBufferLayout & pRenderBufferLayout,
	                                    DXGI_FORMAT pDXGITextureFormat,
	                                    ComPtr<ID3D11Texture2D> pD3D11Texture2D )
	: RenderBuffer( pDX11GPUDevice, pRenderBufferType, pRenderBufferLayout )
	, mDXGITextureFormat( pDXGITextureFormat )
	, mD3D11Device1( pDX11GPUDevice.mD3D11Device1 )
	, mD3D11Texture2D( pD3D11Texture2D )
	{}

	DX11RenderBuffer::~DX11RenderBuffer() = default;

	bool DX11RenderBuffer::isNull() const
	{
		return !mD3D11Texture2D;
	}

	GpaHandle<DX11RenderBuffer> DX11RenderBuffer::create( DX11GPUDevice & pDX11GPUDevice, const RenderBufferCreateInfo & pCreateInfo )
	{
		if( !validateCreateInfo( pCreateInfo ) )
		{
			return nullptr;
		}

		DX11TextureCreateInfo dx11TextureCreateInfo;
		dx11TextureCreateInfo.dimensionClass = ETextureDimensionClass::Texture2D;
		dx11TextureCreateInfo.dimensions.width = pCreateInfo.layout.bufferSize.width;
		dx11TextureCreateInfo.dimensions.height = pCreateInfo.layout.bufferSize.height;
		dx11TextureCreateInfo.dimensions.depth = 1;
		dx11TextureCreateInfo.dimensions.arraySize = 1;
		dx11TextureCreateInfo.dimensions.mipLevelsNum = 1;
		dx11TextureCreateInfo.msaaLevel = pCreateInfo.layout.msaaLevel;
		dx11TextureCreateInfo.dxgiTextureFormat = DX11CoreAPIProxy::translateDXTextureFormat( pCreateInfo.layout.internalDataFormat );
		dx11TextureCreateInfo.dx11UsageDesc.bindFlags = D3D11_BIND_RENDER_TARGET;
		dx11TextureCreateInfo.dx11UsageDesc.cpuAccessFlags = 0;
		dx11TextureCreateInfo.dx11UsageDesc.resourceMiscFlags = 0;
		dx11TextureCreateInfo.dx11UsageDesc.usage = D3D11_USAGE_DEFAULT;

		auto dx11TextureData = DX11Texture::create2D( pDX11GPUDevice, dx11TextureCreateInfo );
		if( !dx11TextureData.d3d11Texture2D )
		{
			return nullptr;
		}

		auto renderBuffer = std::make_shared<DX11RenderBuffer>( pDX11GPUDevice,
		                                                        pCreateInfo.bufferType,
		                                                        pCreateInfo.layout,
		                                                        dx11TextureCreateInfo.dxgiTextureFormat,
		                                                        dx11TextureData.d3d11Texture2D );

		return renderBuffer;
	}

} // namespace ts3::gpuapi
