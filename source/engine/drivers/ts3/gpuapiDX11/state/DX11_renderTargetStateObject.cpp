
#include "DX11_renderTargetStateObject.h"
#include <ts3/gpuapi/resources/texture.h>
#include <ts3/gpuapiDX11/DX11_gpuDevice.h>
#include <ts3/gpuapiDX11/resources/DX11_renderBuffer.h>
#include <ts3/gpuapiDX11/resources/DX11_texture.h>
#include <ts3/stdext/memory.h>

namespace ts3
{
namespace gpuapi
{

	DX11RenderTargetStateObject::DX11RenderTargetStateObject( DX11GPUDevice & pGPUDevice,
	                                                          const RenderTargetLayout & pRTLayout,
	                                                          const RenderTargetResourceBinding & pRTResourceBinding,
	                                                          DX11RenderTargetResourceState pDX11RTResourceState )
	: RenderTargetStateObject( pGPUDevice, pRTLayout, pRTResourceBinding )
	, mDX11RTResourceState( std::move( pDX11RTResourceState ) )
	{}

	DX11RenderTargetStateObject::~DX11RenderTargetStateObject() = default;

	GpaHandle<DX11RenderTargetStateObject> DX11RenderTargetStateObject::create( DX11GPUDevice & pGPUDevice,
	                                                                            const RenderTargetStateObjectCreateInfo & pCreateInfo )
	{
		RenderTargetLayout rtLayout;
		RenderTargetResourceBinding rtResourceBinding;

		if( !createRenderTargetLayoutAndResourceBinding( pCreateInfo.rtResourceBindingDesc, rtLayout, rtResourceBinding ) )
		{
			return nullptr;
		}

		DX11RenderTargetResourceState dx11RTResourceState;
		memZero( dx11RTResourceState.colorAttachmentRTVArray );
		dx11RTResourceState.depthStencilAttachmentDSV = nullptr;

		for( uint32 colorAttachmentIndex = 0; colorAttachmentIndex < GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM; ++colorAttachmentIndex )
		{
			auto & colorAttachmentBinding = rtResourceBinding.colorAttachmentArray[colorAttachmentIndex];
			if( colorAttachmentBinding )
			{
				ComPtr<ID3D11RenderTargetView> d3d11RTV;
				if( colorAttachmentBinding.attachmentResourceType == ERenderTargetResourceType::RenderBuffer )
				{
					auto * dx11RenderBuffer = colorAttachmentBinding.uRenderBufferRef.renderBuffer->queryInterface<DX11RenderBuffer>();
					d3d11RTV = createRTVForRenderBuffer( *dx11RenderBuffer, colorAttachmentBinding );
				}
				else
				{
					auto * dx11TextureStorage = colorAttachmentBinding.uTextureRef.texture->queryInterface<DX11Texture>();
					d3d11RTV = createRTVForTexture( *dx11TextureStorage, colorAttachmentBinding );
				}

				if( !d3d11RTV )
				{
					return nullptr;
				}

				dx11RTResourceState.colorAttachmentViewRef[colorAttachmentIndex] = d3d11RTV;
				dx11RTResourceState.colorAttachmentRTVArray[colorAttachmentIndex] = d3d11RTV.Get();
			}
		}

		auto & depthStencilAttachmentBinding = rtResourceBinding.depthStencilAttachment;
		if( depthStencilAttachmentBinding )
		{
			ComPtr<ID3D11DepthStencilView> d3d11DSV;
			if( depthStencilAttachmentBinding.attachmentResourceType == ERenderTargetResourceType::RenderBuffer )
			{
				auto * dx11RenderBuffer = depthStencilAttachmentBinding.uRenderBufferRef.renderBuffer->queryInterface<DX11RenderBuffer>();
				d3d11DSV = createDSVForRenderBuffer( *dx11RenderBuffer, depthStencilAttachmentBinding );
			}
			else
			{
				auto * dx11Texture = depthStencilAttachmentBinding.uTextureRef.texture->queryInterface<DX11Texture>();
				d3d11DSV = createDSVForTexture( *dx11Texture, depthStencilAttachmentBinding );
			}

			if( !d3d11DSV )
			{
				return nullptr;
			}

			dx11RTResourceState.depthStencilAttachmentViewRef = d3d11DSV;
			dx11RTResourceState.depthStencilAttachmentDSV = d3d11DSV.Get();
		}

		auto dx11RenderTargetSO = std::make_shared<DX11RenderTargetStateObject>( pGPUDevice,
		                                                                         rtLayout,
		                                                                         rtResourceBinding,
		                                                                         std::move( dx11RTResourceState ) );

		return dx11RenderTargetSO;
	}

	ComPtr<ID3D11RenderTargetView> DX11RenderTargetStateObject::createRTVForRenderBuffer( DX11RenderBuffer & pDX11RenderBuffer,
	                                                                                      const RenderTargetAttachmentResourceBinding & pRTBinding )
	{
		auto * d3d11Device1 = pDX11RenderBuffer.mD3D11Device1.Get();
		auto * d3d11Texture2D = pDX11RenderBuffer.mD3D11Texture2D.Get();
		ts3DebugAssert( d3d11Texture2D );

		D3D11_TEXTURE2D_DESC textureDesc;
		d3d11Texture2D->GetDesc( &textureDesc );

		D3D11_RENDER_TARGET_VIEW_DESC d3d11RTVDesc;
		d3d11RTVDesc.Format = textureDesc.Format;
		d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		d3d11RTVDesc.Texture2D.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2D.mipLevel;

		ComPtr<ID3D11RenderTargetView> d3d11TextureRTV;
		auto hResult = d3d11Device1->CreateRenderTargetView( d3d11Texture2D, &d3d11RTVDesc, d3d11TextureRTV.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		return d3d11TextureRTV;
	}

	ComPtr<ID3D11RenderTargetView> DX11RenderTargetStateObject::createRTVForTexture( DX11Texture & pDX11Texture,
	                                                                                 const RenderTargetAttachmentResourceBinding & pRTBinding )
	{
		auto * d3d11Device1 = pDX11Texture.mD3D11Device1.Get();

		D3D11_RENDER_TARGET_VIEW_DESC d3d11RTVDesc;
		d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;

		ID3D11Resource * d3d11Resource = nullptr;

		if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture3D )
		{
			auto * d3d11Texture3D = pDX11Texture.mD3D11Texture3D.Get();
			ts3DebugAssert( d3d11Texture3D );

			D3D11_TEXTURE3D_DESC texture3DDesc;
			d3d11Texture3D->GetDesc( &texture3DDesc );

			d3d11RTVDesc.Format = texture3DDesc.Format;
			d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			d3d11RTVDesc.Texture3D.MipSlice = pRTBinding.uTextureRef.textureSubResource.u3D.mipLevel;
			d3d11RTVDesc.Texture3D.FirstWSlice = pRTBinding.uTextureRef.textureSubResource.u3D.depthLayerIndex;
			d3d11RTVDesc.Texture3D.MipSlice = 1;

			d3d11Resource = d3d11Texture3D;
		}
		else
		{
			auto * d3d11Texture2D = pDX11Texture.mD3D11Texture2D.Get();
			ts3DebugAssert( d3d11Texture2D );

			D3D11_TEXTURE2D_DESC texture2DDesc;
			d3d11Texture2D->GetDesc( &texture2DDesc );

			d3d11RTVDesc.Format = texture2DDesc.Format;

			if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2D )
			{
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				d3d11RTVDesc.Texture2D.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2D.mipLevel;

			}
			else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DArray )
			{
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				d3d11RTVDesc.Texture2DArray.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.mipLevel;
				d3d11RTVDesc.Texture2DArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.arrayIndex;
				d3d11RTVDesc.Texture2DArray.ArraySize = 1;
			}
			else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DMS )
			{
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			}
			else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DMSArray )
			{
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				d3d11RTVDesc.Texture2DMSArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.arrayIndex;
				d3d11RTVDesc.Texture2DMSArray.ArraySize = 1;
			}
			else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::TextureCubeMap )
			{
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				d3d11RTVDesc.Texture2DArray.MipSlice = pRTBinding.uTextureRef.textureSubResource.uCubeMap.mipLevel;
				d3d11RTVDesc.Texture2DArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.uCubeMap.uFaceIndex;
				d3d11RTVDesc.Texture2DArray.ArraySize = 1;
			}
			else
			{
				ts3DebugInterrupt();
				return nullptr;
			}

			d3d11Resource = d3d11Texture2D;
		}

		ComPtr<ID3D11RenderTargetView> d3d11TextureRTV;
		auto hResult = d3d11Device1->CreateRenderTargetView( d3d11Resource, &d3d11RTVDesc, d3d11TextureRTV.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		return d3d11TextureRTV;
	}

	ComPtr<ID3D11DepthStencilView> DX11RenderTargetStateObject::createDSVForRenderBuffer( DX11RenderBuffer & pDX11RenderBuffer,
	                                                                                      const RenderTargetAttachmentResourceBinding & pRTBinding )
	{
		auto * d3d11Device1 = pDX11RenderBuffer.mD3D11Device1.Get();
		auto * d3d11Texture2D = pDX11RenderBuffer.mD3D11Texture2D.Get();
		ts3DebugAssert( d3d11Texture2D );

		D3D11_TEXTURE2D_DESC textureDesc;
		d3d11Texture2D->GetDesc( &textureDesc );

		D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSVDesc;
		d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3d11DSVDesc.Flags = 0;
		d3d11DSVDesc.Format = textureDesc.Format;
		d3d11DSVDesc.Texture2D.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2D.mipLevel;

		ComPtr<ID3D11DepthStencilView> d3d11TextureDSV;
		auto hResult = d3d11Device1->CreateDepthStencilView( d3d11Texture2D, &d3d11DSVDesc, d3d11TextureDSV.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		return d3d11TextureDSV;
	}

	ComPtr<ID3D11DepthStencilView> DX11RenderTargetStateObject::createDSVForTexture( DX11Texture & pDX11Texture,
	                                                                                 const RenderTargetAttachmentResourceBinding & pRTBinding )
	{
		auto * d3d11Device1 = pDX11Texture.mD3D11Device1.Get();
		auto * d3d11Texture2D = pDX11Texture.mD3D11Texture2D.Get();
		ts3DebugAssert( d3d11Texture2D );

		D3D11_TEXTURE2D_DESC texture2DDesc;
		d3d11Texture2D->GetDesc( &texture2DDesc );

		D3D11_DEPTH_STENCIL_VIEW_DESC d3d11DSVDesc;
		d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
		d3d11DSVDesc.Flags = 0;
		d3d11DSVDesc.Format = texture2DDesc.Format;

		if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2D )
		{
			d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			d3d11DSVDesc.Texture2D.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2D.mipLevel;

		}
		else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DArray )
		{
			d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			d3d11DSVDesc.Texture2DArray.MipSlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.mipLevel;
			d3d11DSVDesc.Texture2DArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.arrayIndex;
			d3d11DSVDesc.Texture2DArray.ArraySize = 1;
		}
		else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DMS )
		{
			d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::Texture2DMSArray )
		{
			d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			d3d11DSVDesc.Texture2DMSArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.u2DArray.arrayIndex;
			d3d11DSVDesc.Texture2DMSArray.ArraySize = 1;
		}
		else if( pDX11Texture.mTextureLayout.dimensionClass == ETextureDimensionClass::TextureCubeMap )
		{
			d3d11DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			d3d11DSVDesc.Texture2DArray.MipSlice = pRTBinding.uTextureRef.textureSubResource.uCubeMap.mipLevel;
			d3d11DSVDesc.Texture2DArray.FirstArraySlice = pRTBinding.uTextureRef.textureSubResource.uCubeMap.uFaceIndex;
			d3d11DSVDesc.Texture2DArray.ArraySize = 1;
		}
		else
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		ComPtr<ID3D11DepthStencilView> d3d11TextureDSV;
		auto hResult = d3d11Device1->CreateDepthStencilView( d3d11Texture2D, &d3d11DSVDesc, d3d11TextureDSV.GetAddressOf() );
		if( FAILED( hResult ) )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		return d3d11TextureDSV;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
