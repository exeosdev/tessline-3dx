
#include "DX11_texture.h"
#include <ts3/gpuapiDX11/DX11_apiTranslationLayer.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/DX11_commandList.h>

namespace ts3::gpuapi
{

	DX11Texture::DX11Texture( DX11GPUDevice & pDX11GPUDevice,
	                          const ResourceMemoryInfo & pResourceMemory,
	                          const TextureProperties & pTextureProperties,
	                          const TextureLayout & pTextureLayout,
	                          DXGI_FORMAT pDXGITextureFormat,
	                          ComPtr<ID3D11Texture2D> pD3D11Texture2D )
	: Texture( pDX11GPUDevice, pResourceMemory, pTextureProperties, pTextureLayout )
	, mDXGITextureFormat( pDXGITextureFormat )
	, mD3D11Device1( pDX11GPUDevice.mD3D11Device1 )
	, mD3D11Texture2D( pD3D11Texture2D )
	, mD3D11DefaultSRV( createDefaultSRV2D( pTextureLayout.dimensionClass, mD3D11Texture2D ) )
	{}

	DX11Texture::DX11Texture( DX11GPUDevice & pDX11GPUDevice,
	                          const ResourceMemoryInfo & pResourceMemory,
	                          const TextureProperties & pTextureProperties,
	                          const TextureLayout & pTextureLayout,
	                          DXGI_FORMAT pDXGITextureFormat,
	                          ComPtr<ID3D11Texture3D> pD3D11Texture3D )
	: Texture( pDX11GPUDevice, pResourceMemory, pTextureProperties, pTextureLayout )
	, mDXGITextureFormat( pDXGITextureFormat )
	, mD3D11Device1( pDX11GPUDevice.mD3D11Device1 )
	, mD3D11Texture3D( pD3D11Texture3D )
	, mD3D11DefaultSRV( createDefaultSRV3D( mD3D11Texture3D ) )
	{}

	DX11Texture::~DX11Texture() = default;

	DX11TextureHandle DX11Texture::create( DX11GPUDevice & pDX11GPUDevice, const TextureCreateInfo & pCreateInfo )
	{
		auto createInfo = pCreateInfo;
		if( !validateTextureCreateInfo( createInfo ) )
		{
			return nullptr;
		}

		DX11TextureCreateInfo dx11CreateInfo;
		dx11CreateInfo.dimensionClass = createInfo.dimensionClass;
		dx11CreateInfo.dimensions = createInfo.dimensions;
		dx11CreateInfo.msaaLevel = createInfo.msaaLevel;
		dx11CreateInfo.dxgiTextureFormat = atl::translateTextureFormatDX( createInfo.pixelFormat );
		dx11CreateInfo.dx11UsageDesc = translateTextureUsageDesc( createInfo );
		dx11CreateInfo.dx11InitDataDesc = translateTextureInitDataDesc( pCreateInfo );

		DX11TextureData dx11TextureData = nullptr;

		if( createInfo.dimensionClass == ETextureDimensionClass::Texture2D )
		{
			dx11TextureData = create2D( pDX11GPUDevice, dx11CreateInfo );
		}
		else if( createInfo.dimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			dx11TextureData = create2D( pDX11GPUDevice, dx11CreateInfo );
		}
		else if( createInfo.dimensionClass == ETextureDimensionClass::Texture2DMS )
		{
			dx11TextureData = create2DMS( pDX11GPUDevice, dx11CreateInfo );
		}
		else if( createInfo.dimensionClass == ETextureDimensionClass::Texture3D )
		{
			dx11TextureData = create3D( pDX11GPUDevice, dx11CreateInfo );
		}
		else if( createInfo.dimensionClass == ETextureDimensionClass::TextureCubeMap )
		{
			dx11TextureData = create2D( pDX11GPUDevice, dx11CreateInfo );
		}

		if( !dx11TextureData )
		{
			return nullptr;
		}

		auto textureMemoryByteSize = atl::computeDXTextureMemoryByteSize( dx11CreateInfo.dimensions, dx11CreateInfo.dxgiTextureFormat );

		ResourceMemoryInfo textureMemoryInfo;
		textureMemoryInfo.sourceHeapRegion.offset = 0;
		textureMemoryInfo.sourceHeapRegion.size = textureMemoryByteSize;
		textureMemoryInfo.baseAlignment = createInfo.memoryBaseAlignment;
		textureMemoryInfo.memoryFlags = createInfo.memoryFlags;

		TextureProperties textureProperties;
		textureProperties.resourceFlags = createInfo.resourceFlags;

		TextureLayout textureLayout;
		textureLayout.dimensionClass = createInfo.dimensionClass;
		textureLayout.dimensions = createInfo.dimensions;
		textureLayout.pixelFormat = createInfo.pixelFormat;
		textureLayout.msaaLevel = createInfo.msaaLevel;
		textureLayout.storageSize = textureMemoryByteSize;
		textureLayout.bitsPerPixel = atl::getDXGITextureFormatBPP( dx11CreateInfo.dxgiTextureFormat );

		DX11TextureHandle dx11Texture;

		if( dx11TextureData.d3d11Texture2D )
		{
			dx11Texture = createDynamicInterfaceObject<DX11Texture>( pDX11GPUDevice,
			                                                         textureMemoryInfo,
			                                                         textureProperties,
			                                                         textureLayout,
			                                                         dx11CreateInfo.dxgiTextureFormat,
			                                                         dx11TextureData.d3d11Texture2D );
		}
		else if( dx11TextureData.d3d11Texture3D )
		{
			dx11Texture = createDynamicInterfaceObject<DX11Texture>( pDX11GPUDevice,
			                                                         textureMemoryInfo,
			                                                         textureProperties,
			                                                         textureLayout,
			                                                         dx11CreateInfo.dxgiTextureFormat,
			                                                         dx11TextureData.d3d11Texture3D );
		}

		return dx11Texture;
	}

	ComPtr<ID3D11ShaderResourceView> DX11Texture::createDefaultSRV2D( ETextureDimensionClass pDimensionClass,
	                                                                         const ComPtr<ID3D11Texture2D> & pD3D11Texture2D )
	{
		ComPtr<ID3D11Device> d3d11Device;
		pD3D11Texture2D->GetDevice( d3d11Device.GetAddressOf() );

		D3D11_TEXTURE2D_DESC d3D11Texture2DDesc;
		pD3D11Texture2D->GetDesc( &d3D11Texture2DDesc );

		D3D11_SHADER_RESOURCE_VIEW_DESC d3d11DefaultSRVDesc;
		d3d11DefaultSRVDesc.Format = d3D11Texture2DDesc.Format;

		if( pDimensionClass == ETextureDimensionClass::Texture2D )
		{
			d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			d3d11DefaultSRVDesc.Texture2D.MipLevels = d3D11Texture2DDesc.MipLevels;
			d3d11DefaultSRVDesc.Texture2D.MostDetailedMip = 0;
		}
		else if( pDimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			d3d11DefaultSRVDesc.Texture2DArray.ArraySize = d3D11Texture2DDesc.ArraySize;
			d3d11DefaultSRVDesc.Texture2DArray.FirstArraySlice = 0;
			d3d11DefaultSRVDesc.Texture2DArray.MipLevels = d3D11Texture2DDesc.MipLevels;
			d3d11DefaultSRVDesc.Texture2DArray.MostDetailedMip = 0;
		}
		else if( pDimensionClass == ETextureDimensionClass::TextureCubeMap )
		{
			d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			d3d11DefaultSRVDesc.TextureCube.MipLevels = d3D11Texture2DDesc.MipLevels;
			d3d11DefaultSRVDesc.TextureCube.MostDetailedMip = 0;
		}

		ComPtr<ID3D11ShaderResourceView> d3d11DefaultSRV;
		auto hResult = d3d11Device->CreateShaderResourceView( pD3D11Texture2D.Get(),
		                                                      &d3d11DefaultSRVDesc,
		                                                      d3d11DefaultSRV.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11DefaultSRV;
	}

	ComPtr<ID3D11ShaderResourceView> DX11Texture::createDefaultSRV3D( const ComPtr<ID3D11Texture3D> & pD3D11Texture3D )
	{
		ComPtr<ID3D11Device> d3d11Device;
		pD3D11Texture3D->GetDevice( d3d11Device.GetAddressOf() );

		D3D11_TEXTURE3D_DESC d3D11Texture3DDesc;
		pD3D11Texture3D->GetDesc( &d3D11Texture3DDesc );

		D3D11_SHADER_RESOURCE_VIEW_DESC d3d11DefaultSRVDesc;
		d3d11DefaultSRVDesc.Format = d3D11Texture3DDesc.Format;
		d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		d3d11DefaultSRVDesc.Texture3D.MipLevels = d3D11Texture3DDesc.MipLevels;
		d3d11DefaultSRVDesc.Texture3D.MostDetailedMip = 0;

		ComPtr<ID3D11ShaderResourceView> d3d11DefaultSRV;
		auto hResult = d3d11Device->CreateShaderResourceView( pD3D11Texture3D.Get(),
		                                                      &d3d11DefaultSRVDesc,
		                                                      d3d11DefaultSRV.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11DefaultSRV;
	}

	DX11Texture::DX11TextureData DX11Texture::create2D( DX11GPUDevice & pDX11GPUDevice,
	                                                                  const DX11TextureCreateInfo & pDX11CreateInfo )
	{
		D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
		d3d11Texture2DDesc.Format = pDX11CreateInfo.dxgiTextureFormat;
		d3d11Texture2DDesc.Width = pDX11CreateInfo.dimensions.width;
		d3d11Texture2DDesc.Height = pDX11CreateInfo.dimensions.height;
		d3d11Texture2DDesc.ArraySize = pDX11CreateInfo.dimensions.arraySize;
		d3d11Texture2DDesc.MipLevels = pDX11CreateInfo.dimensions.mipLevelsNum;
		d3d11Texture2DDesc.BindFlags = pDX11CreateInfo.dx11UsageDesc.bindFlags;
		d3d11Texture2DDesc.CPUAccessFlags = pDX11CreateInfo.dx11UsageDesc.cpuAccessFlags;
		d3d11Texture2DDesc.MiscFlags = pDX11CreateInfo.dx11UsageDesc.resourceMiscFlags;
		d3d11Texture2DDesc.Usage = pDX11CreateInfo.dx11UsageDesc.usage;
		d3d11Texture2DDesc.SampleDesc.Count = 1;
		d3d11Texture2DDesc.SampleDesc.Quality = 0;

		const D3D11_SUBRESOURCE_DATA * d3d11TextureInitData = nullptr;

		if( pDX11CreateInfo.dx11InitDataDesc )
		{
			const auto textureSubResourcesNum = d3d11Texture2DDesc.ArraySize * d3d11Texture2DDesc.MipLevels;
			const auto initDataEntriesNum = pDX11CreateInfo.dx11InitDataDesc.d3d11SubResourceDataArray.size();

			if( initDataEntriesNum != textureSubResourcesNum )
			{
				ts3DebugInterrupt();
				ts3DebugOutput( "Warn: init data must fill the whole resource. Texture will be created with default content." );
			}
			else
			{
				d3d11TextureInitData = pDX11CreateInfo.dx11InitDataDesc.d3d11SubResourceDataArray.data();
			}
		}

		ComPtr<ID3D11Texture2D> d3d11Texture2D;
		auto hResult = pDX11GPUDevice.mD3D11Device1->CreateTexture2D( &d3d11Texture2DDesc,
		                                                              d3d11TextureInitData,
		                                                              d3d11Texture2D.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		return d3d11Texture2D;
	}

	DX11Texture::DX11TextureData DX11Texture::create2DMS( DX11GPUDevice & pDX11GPUDevice,
	                                                                    const DX11TextureCreateInfo & pDX11CreateInfo )
	{
		return nullptr;
	}

	DX11Texture::DX11TextureData DX11Texture::create3D( DX11GPUDevice & pDX11GPUDevice,
	                                                                  const DX11TextureCreateInfo & pDX11CreateInfo )
	{
		return nullptr;
	}

	DX11TextureInitDataDesc DX11Texture::translateTextureInitDataDesc( const TextureCreateInfo & pCreateInfo )
	{
		DX11TextureInitDataDesc dx11InitDataDesc;

		if( pCreateInfo.initDataDesc )
		{
			auto subResourcesNum = pCreateInfo.dimensions.arraySize * pCreateInfo.dimensions.mipLevelsNum;
			dx11InitDataDesc.textureSubResourcesNum = subResourcesNum;
			dx11InitDataDesc.d3d11SubResourceDataArray.resize( subResourcesNum );
			dx11InitDataDesc.pixelByteSize = ecGetTextureFormatByteSize( pCreateInfo.pixelFormat );

			for( uint32 subTextureIndex = 0; subTextureIndex < pCreateInfo.dimensions.arraySize; ++subTextureIndex )
			{
				const auto & subTextureInitData = pCreateInfo.initDataDesc.subTextureInitDataBasePtr[subTextureIndex];

				for( uint32 mipLevelIndex = 0; mipLevelIndex < pCreateInfo.dimensions.mipLevelsNum; ++mipLevelIndex )
				{
					const auto & mipLevelInitData = subTextureInitData.mipLevelInitDataArray[mipLevelIndex];
					const auto d3d11TextureSubResourceIndex = subTextureIndex * pCreateInfo.dimensions.mipLevelsNum + mipLevelIndex;

					auto & d3d11SubResourceData = dx11InitDataDesc.d3d11SubResourceDataArray[d3d11TextureSubResourceIndex];
					d3d11SubResourceData.pSysMem = mipLevelInitData.pointer;
					d3d11SubResourceData.SysMemPitch = mipLevelInitData.mipWidth * dx11InitDataDesc.pixelByteSize;
					d3d11SubResourceData.SysMemSlicePitch = d3d11SubResourceData.SysMemPitch * mipLevelInitData.mipHeight;
				}
			}
		}

		return dx11InitDataDesc;
	}

	DX11TextureUsageDesc DX11Texture::translateTextureUsageDesc( const TextureCreateInfo & pCreateInfo )
	{
		DX11TextureUsageDesc dx11UsageDesc;
		dx11UsageDesc.bindFlags = atl::translateDX11ETextureBindFlags( pCreateInfo.resourceFlags );
		dx11UsageDesc.cpuAccessFlags = 0;
		dx11UsageDesc.resourceMiscFlags = 0;
		dx11UsageDesc.usage = D3D11_USAGE_DEFAULT;

		if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_IMMUTABLE_BIT ) )
		{
			dx11UsageDesc.usage = D3D11_USAGE_IMMUTABLE;
		}
		if( pCreateInfo.resourceFlags.isSetAnyOf( E_GPU_RESOURCE_USAGE_FLAG_TRANSFER_SOURCE_BIT ) )
		{
			dx11UsageDesc.usage = D3D11_USAGE_STAGING;
		}
		if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_CONTENT_FLAG_DYNAMIC_BIT ) && ( dx11UsageDesc.usage != D3D11_USAGE_DYNAMIC ) )
		{
			dx11UsageDesc.usage = D3D11_USAGE_DYNAMIC;
			dx11UsageDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_WRITE );
		}

		if( pCreateInfo.memoryFlags.isSet( E_GPU_MEMORY_ACCESS_FLAG_CPU_READ_BIT ) )
		{
			dx11UsageDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_READ );
		}
		if( pCreateInfo.memoryFlags.isSet( E_GPU_MEMORY_ACCESS_FLAG_CPU_WRITE_BIT ) )
		{
			dx11UsageDesc.cpuAccessFlags.set( D3D11_CPU_ACCESS_WRITE );
		}

		return dx11UsageDesc;
	}

	ID3D11DeviceContext1 * DX11Texture::getD3D11DeviceContext( void * pCommandObject )
	{
		return reinterpret_cast<DX11CommandList *>( pCommandObject )->mD3D11DeviceContext1.Get();
	}

} // namespace ts3::gpuapi
