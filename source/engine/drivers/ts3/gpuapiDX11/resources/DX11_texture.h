
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_TEXTURE_H__
#define __TS3DRIVER_GPUAPI_DX11_TEXTURE_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX11Texture );

	class DX11RenderBuffer;

	struct DX11TextureUsageDesc
	{
		Bitmask<D3D11_BIND_FLAG> bindFlags;
		Bitmask<D3D11_CPU_ACCESS_FLAG> cpuAccessFlags;
		Bitmask<D3D11_RESOURCE_MISC_FLAG> resourceMiscFlags;
		D3D11_USAGE usage;
	};

	struct DX11TextureInitDataDesc
	{
		std::vector<D3D11_SUBRESOURCE_DATA> d3d11SubResourceDataArray;
		uint32 textureSubResourcesNum;
		uint32 pixelByteSize;

		explicit operator bool() const
		{
			return !d3d11SubResourceDataArray.empty();
		}
	};

	struct DX11TextureCreateInfo
	{
		ETextureDimensionClass dimensionClass;
		TextureDimensions dimensions;
		DXGI_FORMAT dxgiTextureFormat;
		UINT msaaLevel;
		DX11TextureUsageDesc dx11UsageDesc;
		DX11TextureInitDataDesc dx11InitDataDesc;
	};

	class DX11Texture : public Texture
	{
		friend class DX11CommandContext;
		friend class DX11RenderBuffer;

	public:
		DXGI_FORMAT const mDXGITextureFormat;
		ComPtr<ID3D11Device1> const mD3D11Device1;
		ComPtr<ID3D11Texture2D> const mD3D11Texture2D;
		ComPtr<ID3D11Texture3D> const mD3D11Texture3D;
		ComPtr<ID3D11ShaderResourceView> const mD3D11DefaultSRV;

	public:
		DX11Texture( DX11GPUDevice & pDX11GPUDevice,
		             const ResourceMemoryInfo & pResourceMemory,
		             const TextureProperties & pTextureProperties,
		             const TextureLayout & pTextureLayout,
		             DXGI_FORMAT pDXGITextureFormat,
		             ComPtr<ID3D11Texture2D> pD3D11Texture2D );

		DX11Texture( DX11GPUDevice & pDX11GPUDevice,
		             const ResourceMemoryInfo & pResourceMemory,
		             const TextureProperties & pTextureProperties,
		             const TextureLayout & pTextureLayout,
		             DXGI_FORMAT pDXGITextureFormat,
		             ComPtr<ID3D11Texture3D> pD3D11Texture3D );

		virtual ~DX11Texture();

		static DX11TextureHandle create( DX11GPUDevice & pDX11GPUDevice, const TextureCreateInfo & pCreateInfo );

	private:
		struct DX11TextureData
		{
			ComPtr<ID3D11Texture2D> d3d11Texture2D;
			ComPtr<ID3D11Texture3D> d3d11Texture3D;

			DX11TextureData( const ComPtr<ID3D11Texture2D> & pD3D11Texture2D )
			: d3d11Texture2D( pD3D11Texture2D )
			{}

			DX11TextureData( const ComPtr<ID3D11Texture3D> & pD3D11Texture3D )
			: d3d11Texture3D( pD3D11Texture3D )
			{}

			DX11TextureData( std::nullptr_t )
			{}

			explicit operator bool() const
			{
				return d3d11Texture2D || d3d11Texture3D;
			}
		};

		static ComPtr<ID3D11ShaderResourceView> createDefaultSRV2D( ETextureDimensionClass pDimensionClass,
		                                                            const ComPtr<ID3D11Texture2D> & pD3D11Texture2D );

		static ComPtr<ID3D11ShaderResourceView> createDefaultSRV3D( const ComPtr<ID3D11Texture3D> & pD3D11Texture3D );

		static DX11TextureData create2D( DX11GPUDevice & pDX11GPUDevice, const DX11TextureCreateInfo & pDX11CreateInfo );
		static DX11TextureData create2DMS( DX11GPUDevice & pDX11GPUDevice, const DX11TextureCreateInfo & pDX11CreateInfo );
		static DX11TextureData create3D( DX11GPUDevice & pDX11GPUDevice, const DX11TextureCreateInfo & pDX11CreateInfo );

		static DX11TextureInitDataDesc translateTextureInitDataDesc( const TextureCreateInfo & pCreateInfo );
		static DX11TextureUsageDesc translateTextureUsageDesc( const TextureCreateInfo & pCreateInfo );

		static ID3D11DeviceContext1 * getD3D11DeviceContext( void * pCommandObject );
	};

}

#endif // __TS3DRIVER_GPUAPI_DX11_TEXTURE_H__
