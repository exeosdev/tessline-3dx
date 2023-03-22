
#include "DX11_texture.h"
#include <ts3/gpuapiDX11/DX11_apiTranslationLayer.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/DX11_commandList.h>
#include <ts3/gpuapi/resources/renderTargetTexture.h>

#include <ts3/system/sysContextNative.h>

namespace ts3::gpuapi
{

	DX11Texture::DX11Texture(
			DX11GPUDevice & pDX11GPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const TextureProperties & pTextureProperties,
			const TextureLayout & pTextureLayout,
			DXGI_FORMAT pDXGIInternalFormat,
			ComPtr<ID3D11Texture2D> pD3D11Texture2D,
			ComPtr<ID3D11ShaderResourceView> pD3D11DefaultSRV )
	: Texture( pDX11GPUDevice, pResourceMemory, pTextureProperties, pTextureLayout )
	, mDXGIInternalFormat( pDXGIInternalFormat )
	, mD3D11Device1( pDX11GPUDevice.mD3D11Device1 )
	, mD3D11Texture2D( pD3D11Texture2D )
	, mD3D11DefaultSRV( pD3D11DefaultSRV )
	{}

	DX11Texture::DX11Texture(
			DX11GPUDevice & pDX11GPUDevice,
			const ResourceMemoryInfo & pResourceMemory,
			const TextureProperties & pTextureProperties,
			const TextureLayout & pTextureLayout,
			DXGI_FORMAT pDXGIInternalFormat,
			ComPtr<ID3D11Texture3D> pD3D11Texture3D,
			ComPtr<ID3D11ShaderResourceView> pD3D11DefaultSRV )
	: Texture( pDX11GPUDevice, pResourceMemory, pTextureProperties, pTextureLayout )
	, mDXGIInternalFormat( pDXGIInternalFormat )
	, mD3D11Device1( pDX11GPUDevice.mD3D11Device1 )
	, mD3D11Texture3D( pD3D11Texture3D )
	, mD3D11DefaultSRV( pD3D11DefaultSRV )
	{}

	DX11Texture::~DX11Texture() = default;

	DX11TextureHandle DX11Texture::createDefault(
			DX11GPUDevice & pDX11GPUDevice,
			const TextureCreateInfo & pCreateInfo )
	{
		auto dxgiTextureFormat = atl::translateTextureFormatDX( pCreateInfo.internalFormat );
		if( pCreateInfo.resourceFlags.isSetAnyOf( E_GPU_RESOURCE_USAGE_MASK_RENDER_TARGET_DEPTH_STENCIL ) && pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT ) )
		{
			dxgiTextureFormat = rcutil::getTypelessFormatForDpethStencilFormatDX11( dxgiTextureFormat );
		}

		DX11TextureCreateInfo dx11CreateInfo;
		dx11CreateInfo.texClass = pCreateInfo.texClass;
		dx11CreateInfo.dimensions = rcutil::getValidTextureDimensions( pCreateInfo.texClass, pCreateInfo.dimensions );
		dx11CreateInfo.msaaLevel = pCreateInfo.msaaLevel;
		dx11CreateInfo.resourceFlags = pCreateInfo.resourceFlags;
		dx11CreateInfo.dxgiTextureFormat = dxgiTextureFormat;
		dx11CreateInfo.dx11UsageDesc = rcutil::translateTextureUsageDescDX11( pCreateInfo );
		dx11CreateInfo.dx11InitDataDesc = rcutil::translateTextureInitDataDescDX11( pCreateInfo );

		auto dx11TextureData = rcutil::createTextureResourceDX11( pDX11GPUDevice, dx11CreateInfo );
		if( !dx11TextureData )
		{
			return nullptr;
		}

		auto textureMemoryByteSize = atl::computeDXTextureMemoryByteSize( dx11CreateInfo.dimensions, dx11CreateInfo.dxgiTextureFormat );

		ResourceMemoryInfo textureMemoryInfo;
		textureMemoryInfo.sourceHeapRegion.offset = 0;
		textureMemoryInfo.sourceHeapRegion.size = textureMemoryByteSize;
		textureMemoryInfo.baseAlignment = pCreateInfo.memoryBaseAlignment;
		textureMemoryInfo.memoryFlags = pCreateInfo.memoryFlags;

		TextureProperties textureProperties;
		textureProperties.resourceFlags = pCreateInfo.resourceFlags;

		TextureLayout textureLayout;
		textureLayout.texClass = pCreateInfo.texClass;
		textureLayout.dimensions = pCreateInfo.dimensions;
		textureLayout.internalFormat = pCreateInfo.internalFormat;
		textureLayout.msaaLevel = pCreateInfo.msaaLevel;
		textureLayout.storageSize = textureMemoryByteSize;
		textureLayout.bitsPerPixel = atl::getDXGITextureFormatBPP( dx11CreateInfo.dxgiTextureFormat );

		DX11TextureHandle dx11Texture;

		if( dx11TextureData.d3d11Texture2D )
		{
			ComPtr<ID3D11ShaderResourceView> d3d11TextureSRView = nullptr;
			if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT ) )
			{
				d3d11TextureSRView = rcutil::create2DTextureDefaultShaderResourceViewDX11( pCreateInfo.texClass, dx11TextureData.d3d11Texture2D );
			}

			dx11Texture = createDynamicInterfaceObject<DX11Texture>(
					pDX11GPUDevice,
					textureMemoryInfo,
					textureProperties,
					textureLayout,
					dx11CreateInfo.dxgiTextureFormat,
					dx11TextureData.d3d11Texture2D,
					d3d11TextureSRView );
		}
		else if( dx11TextureData.d3d11Texture3D )
		{
			ComPtr<ID3D11ShaderResourceView> d3d11TextureSRView = nullptr;
			if( pCreateInfo.resourceFlags.isSet( E_GPU_RESOURCE_USAGE_FLAG_SHADER_INPUT_BIT ) )
			{
				d3d11TextureSRView = rcutil::create3DTextureDefaultShaderResourceViewDX11( dx11TextureData.d3d11Texture3D );
			}

			dx11Texture = createDynamicInterfaceObject<DX11Texture>(
					pDX11GPUDevice,
					textureMemoryInfo,
					textureProperties,
					textureLayout,
					dx11CreateInfo.dxgiTextureFormat,
					dx11TextureData.d3d11Texture3D,
					d3d11TextureSRView );
		}

		return dx11Texture;
	}

	DX11TextureHandle DX11Texture::createForRenderTarget(
			DX11GPUDevice & pDX11GPUDevice,
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		TextureCreateInfo textureCreateInfo;
		textureCreateInfo.texClass = ETextureClass::T2D;
		textureCreateInfo.resourceFlags = static_cast<resource_flags_value_t>( pCreateInfo.bindFlags );
		textureCreateInfo.memoryFlags = E_GPU_MEMORY_ACCESS_FLAG_GPU_READ_BIT;
		textureCreateInfo.internalFormat = pCreateInfo.rtTextureLayout.internalFormat;
		textureCreateInfo.msaaLevel = pCreateInfo.rtTextureLayout.msaaLevel;
		textureCreateInfo.dimensions.width = pCreateInfo.rtTextureLayout.imageRect.width;
		textureCreateInfo.dimensions.height = pCreateInfo.rtTextureLayout.imageRect.height;

		auto dx11Texture = createDefault( pDX11GPUDevice, textureCreateInfo );

		return dx11Texture;

	}

	RenderTargetTextureHandle DX11Texture::createRenderTargetTextureView(
			DX11GPUDevice & pDX11GPUDevice,
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		if( pCreateInfo.targetTexture )
		{
			return createDefaultRenderTargetTextureView( pDX11GPUDevice, pCreateInfo );
		}
		else
		{
			auto dx11Texture = createForRenderTarget( pDX11GPUDevice, pCreateInfo );

			const auto rtTextureType = rcutil::queryRenderTargetTextureType( dx11Texture->mTextureLayout.internalFormat );
			const auto rtTextureLayout = rcutil::queryRenderTargetTextureLayout( dx11Texture->mTextureLayout );

			auto renderTargetTexture = createGPUAPIObject<RenderTargetTexture>(
					pDX11GPUDevice,
					rtTextureType,
					rtTextureLayout,
					TextureReference{ dx11Texture } );

			return renderTargetTexture;
		}
	}


	namespace rcutil
	{

		DX11TextureData createTextureResourceDX11(
				DX11GPUDevice & pDX11GPUDevice,
				const DX11TextureCreateInfo & pCreateInfo )
		{
			DX11TextureData dx11TextureData = nullptr;

			if( pCreateInfo.texClass == ETextureClass::T2D )
			{
				dx11TextureData = create2DTextureResourceDX11( pDX11GPUDevice, pCreateInfo );
			}
			else if( pCreateInfo.texClass == ETextureClass::T2DArray )
			{
				dx11TextureData = create2DTextureResourceDX11( pDX11GPUDevice, pCreateInfo );
			}
			else if( pCreateInfo.texClass == ETextureClass::T2DMS )
			{
				dx11TextureData = create2DMSTextureResourceDX11( pDX11GPUDevice, pCreateInfo );
			}
			else if( pCreateInfo.texClass == ETextureClass::T3D )
			{
				dx11TextureData = create3DTextureResourceDX11( pDX11GPUDevice, pCreateInfo );
			}
			else if( pCreateInfo.texClass == ETextureClass::TCubeMap )
			{
				dx11TextureData = create2DTextureResourceDX11( pDX11GPUDevice, pCreateInfo );
			}

			return dx11TextureData;
		}

		DX11TextureData create2DTextureResourceDX11(
				DX11GPUDevice & pDX11GPUDevice,
				const DX11TextureCreateInfo & pCreateInfo )
		{
			D3D11_TEXTURE2D_DESC d3d11Texture2DDesc;
			d3d11Texture2DDesc.Format = pCreateInfo.dxgiTextureFormat;
			d3d11Texture2DDesc.Width = pCreateInfo.dimensions.width;
			d3d11Texture2DDesc.Height = pCreateInfo.dimensions.height;
			d3d11Texture2DDesc.ArraySize = pCreateInfo.dimensions.arraySize;
			d3d11Texture2DDesc.MipLevels = pCreateInfo.dimensions.mipLevelsNum;
			d3d11Texture2DDesc.BindFlags = pCreateInfo.dx11UsageDesc.bindFlags;
			d3d11Texture2DDesc.CPUAccessFlags = pCreateInfo.dx11UsageDesc.cpuAccessFlags;
			d3d11Texture2DDesc.MiscFlags = pCreateInfo.dx11UsageDesc.resourceMiscFlags;
			d3d11Texture2DDesc.Usage = pCreateInfo.dx11UsageDesc.usage;
			d3d11Texture2DDesc.SampleDesc.Count = 1;
			d3d11Texture2DDesc.SampleDesc.Quality = 0;

			const D3D11_SUBRESOURCE_DATA * d3d11TextureInitData = nullptr;

			if( pCreateInfo.dx11InitDataDesc )
			{
				const auto textureSubResourcesNum = d3d11Texture2DDesc.ArraySize * d3d11Texture2DDesc.MipLevels;
				const auto initDataEntriesNum = pCreateInfo.dx11InitDataDesc.d3d11SubResourceDataArray.size();

				if( initDataEntriesNum != textureSubResourcesNum )
				{
					ts3DebugInterrupt();
					ts3DebugOutput( "Warn: init data must fill the whole resource. Texture will be created with default content." );
				}
				else
				{
					d3d11TextureInitData = pCreateInfo.dx11InitDataDesc.d3d11SubResourceDataArray.data();
				}
			}

			ComPtr<ID3D11Texture2D> d3d11Texture2D;
			auto hResult = pDX11GPUDevice.mD3D11Device1->CreateTexture2D(
					&d3d11Texture2DDesc,
					d3d11TextureInitData,
					d3d11Texture2D.GetAddressOf() );

			if( FAILED( hResult ) )
			{
				const auto errStr = system::platform::mseQueryCOMErrorMessage(hResult);
				ts3DebugInterrupt();
				return nullptr;
			}

			return d3d11Texture2D;
		}

		DX11TextureData create2DMSTextureResourceDX11(
				DX11GPUDevice & pDX11GPUDevice,
				const DX11TextureCreateInfo & pCreateInfo )
		{
			return nullptr;
		}

		DX11TextureData create3DTextureResourceDX11(
				DX11GPUDevice & pDX11GPUDevice,
				const DX11TextureCreateInfo & pCreateInfo )
		{
			return nullptr;
		}

		ComPtr<ID3D11ShaderResourceView> create2DTextureDefaultShaderResourceViewDX11(
				ETextureClass pTexClass,
				const ComPtr<ID3D11Texture2D> & pD3D11Texture2D )
		{
			ComPtr<ID3D11Device> d3d11Device;
			pD3D11Texture2D->GetDevice( d3d11Device.GetAddressOf() );

			D3D11_TEXTURE2D_DESC d3D11Texture2DDesc;
			pD3D11Texture2D->GetDesc( &d3D11Texture2DDesc );

			D3D11_SHADER_RESOURCE_VIEW_DESC d3d11DefaultSRVDesc;
			d3d11DefaultSRVDesc.Format = d3D11Texture2DDesc.Format;

			if( rcutil::checkIsDXGIFormatTypelessDX11( d3d11DefaultSRVDesc.Format ) )
			{
				d3d11DefaultSRVDesc.Format = rcutil::getSRVFormatForTypelessFormatDX11( d3d11DefaultSRVDesc.Format, E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT | E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT );
			}

			if( pTexClass == ETextureClass::T2D )
			{
				d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				d3d11DefaultSRVDesc.Texture2D.MipLevels = d3D11Texture2DDesc.MipLevels;
				d3d11DefaultSRVDesc.Texture2D.MostDetailedMip = 0;
			}
			else if( pTexClass == ETextureClass::T2DArray )
			{
				d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				d3d11DefaultSRVDesc.Texture2DArray.ArraySize = d3D11Texture2DDesc.ArraySize;
				d3d11DefaultSRVDesc.Texture2DArray.FirstArraySlice = 0;
				d3d11DefaultSRVDesc.Texture2DArray.MipLevels = d3D11Texture2DDesc.MipLevels;
				d3d11DefaultSRVDesc.Texture2DArray.MostDetailedMip = 0;
			}
			else if( pTexClass == ETextureClass::TCubeMap )
			{
				d3d11DefaultSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				d3d11DefaultSRVDesc.TextureCube.MipLevels = d3D11Texture2DDesc.MipLevels;
				d3d11DefaultSRVDesc.TextureCube.MostDetailedMip = 0;
			}

			ComPtr<ID3D11ShaderResourceView> d3d11TextureSRV;
			auto hResult = d3d11Device->CreateShaderResourceView(
					pD3D11Texture2D.Get(),
					&d3d11DefaultSRVDesc,
					d3d11TextureSRV.GetAddressOf() );

			if( FAILED( hResult ) )
			{
				ts3DebugInterrupt();
				return nullptr;
			}

			return d3d11TextureSRV;
		}

		ComPtr<ID3D11ShaderResourceView> create3DTextureDefaultShaderResourceViewDX11(
				const ComPtr<ID3D11Texture3D> & pD3D11Texture3D )
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

			if( rcutil::checkIsDXGIFormatTypelessDX11( d3d11DefaultSRVDesc.Format ) )
			{
				d3d11DefaultSRVDesc.Format = rcutil::getSRVFormatForTypelessFormatDX11( d3d11DefaultSRVDesc.Format, E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT );
			}

			ComPtr<ID3D11ShaderResourceView> d3d11TextureSRV;
			auto hResult = d3d11Device->CreateShaderResourceView(
					pD3D11Texture3D.Get(),
					&d3d11DefaultSRVDesc,
					d3d11TextureSRV.GetAddressOf() );

			if( FAILED( hResult ) )
			{
				ts3DebugInterrupt();
				return nullptr;
			}

			return d3d11TextureSRV;
		}

		bool checkIsDXGIFormatDepthStencilDX11( DXGI_FORMAT pDXGIFormat )
		{
			switch( pDXGIFormat )
			{
				case DXGI_FORMAT_D32_FLOAT:
				case DXGI_FORMAT_D24_UNORM_S8_UINT:
				case DXGI_FORMAT_D16_UNORM:
					return true;
			}

			return false;
		}

		bool checkIsDXGIFormatTypelessDX11( DXGI_FORMAT pDXGIFormat )
		{
			switch( pDXGIFormat )
			{
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_R16_TYPELESS:
				return true;
			}

			return false;

		}

		DXGI_FORMAT getTypelessFormatForDpethStencilFormatDX11( DXGI_FORMAT pDXGIFormat )
		{
			switch( pDXGIFormat )
			{
				ts3CaseReturn( DXGI_FORMAT_D32_FLOAT            , DXGI_FORMAT_R32_TYPELESS );
				ts3CaseReturn( DXGI_FORMAT_D24_UNORM_S8_UINT    , DXGI_FORMAT_R24G8_TYPELESS );
				ts3CaseReturn( DXGI_FORMAT_D16_UNORM            , DXGI_FORMAT_R16_TYPELESS );
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		DXGI_FORMAT getDSVFormatForTypelessFormatDX11( DXGI_FORMAT pDXGIFormat )
		{
			if( pDXGIFormat == DXGI_FORMAT_R32_TYPELESS )
			{
				return DXGI_FORMAT_D32_FLOAT;
			}

			if( pDXGIFormat == DXGI_FORMAT_R24G8_TYPELESS )
			{
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			}

			if( pDXGIFormat == DXGI_FORMAT_R16_TYPELESS )
			{
				return DXGI_FORMAT_D16_UNORM;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		DXGI_FORMAT getSRVFormatForTypelessFormatDX11( DXGI_FORMAT pDXGIFormat, Bitmask<ERenderTargetBufferFlags> pRTBufferMask )
		{
			if( pDXGIFormat == DXGI_FORMAT_R32_TYPELESS )
			{
				return DXGI_FORMAT_R32_FLOAT;
			}

			if( pDXGIFormat == DXGI_FORMAT_R24G8_TYPELESS )
			{
				if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT | E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT ) )
				{
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				}
				if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_DEPTH_BIT ) )
				{
					return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				}
				if( pRTBufferMask.isSet( E_RENDER_TARGET_BUFFER_FLAG_STENCIL_BIT ) )
				{
					return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
				}
			}

			if( pDXGIFormat == DXGI_FORMAT_R16_TYPELESS )
			{
				return DXGI_FORMAT_R16_UNORM;
			}

			return DXGI_FORMAT_UNKNOWN;
		}

		DX11TextureInitDataDesc translateTextureInitDataDescDX11( const TextureCreateInfo & pCreateInfo )
		{
			DX11TextureInitDataDesc dx11InitDataDesc;

			if( pCreateInfo.initDataDesc )
			{
				auto subResourcesNum = pCreateInfo.dimensions.arraySize * pCreateInfo.dimensions.mipLevelsNum;
				dx11InitDataDesc.textureSubResourcesNum = subResourcesNum;
				dx11InitDataDesc.d3d11SubResourceDataArray.resize( subResourcesNum );
				dx11InitDataDesc.pixelByteSize = cxdefs::getTextureFormatByteSize( pCreateInfo.internalFormat );

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

		DX11TextureUsageDesc translateTextureUsageDescDX11( const TextureCreateInfo & pCreateInfo )
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

	}

} // namespace ts3::gpuapi
