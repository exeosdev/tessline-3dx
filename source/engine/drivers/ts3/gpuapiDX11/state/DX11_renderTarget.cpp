
#include "DX11_renderTarget.h"
#include "../DX11_gpuDevice.h"
#include "../resources/DX11_texture.h"
#include <ts3/gpuapi/resources/renderTargetTexture.h>

namespace ts3::gpuapi
{

	DX11RenderTargetBindingImmutableState::DX11RenderTargetBindingImmutableState(
			DX11GPUDevice & pGPUDevice,
			const RenderTargetLayout & pRenderTargetLayout,
			DX11RenderTargetResourceState pDX11RTResourceState )
	: RenderTargetBindingImmutableState( pGPUDevice, pRenderTargetLayout )
	, mDX11RTResourceState( pDX11RTResourceState )
	{}

	DX11RenderTargetBindingImmutableState::~DX11RenderTargetBindingImmutableState() = default;

	GpaHandle<DX11RenderTargetBindingImmutableState> DX11RenderTargetBindingImmutableState::createInstance(
			DX11GPUDevice & pGPUDevice,
			const RenderTargetBindingDefinition & pBindingDefinition )
	{
	}

	GpaHandle<DX11RenderTargetBindingImmutableState> DX11RenderTargetBindingImmutableState::createForScreen(
			DX11GPUDevice & pGPUDevice,
			ComPtr<IDXGISwapChain1> pDXGISwapChain )
	{
	}


	namespace smutil
	{

		ComPtr<ID3D11RenderTargetView> createTextureColorAttachmentView( const RenderTargetAttachmentBinding & pAttachmentBinding )
		{
			const auto & targetTextureRef = pAttachmentBinding.attachmentTexture->getTargetTextureRef();

			auto * dx11Texture = targetTextureRef->queryInterface<DX11Texture>();
			auto * d3d11Device1 = dx11Texture->mD3D11Device1.Get();

			D3D11_RENDER_TARGET_VIEW_DESC d3d11RTVDesc;
			d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;

			ID3D11Resource * d3d11Resource = nullptr;

			if( dx11Texture->mTextureLayout.texClass == ETextureClass::T3D )
			{
				auto * d3d11Texture3D = dx11Texture->mD3D11Texture3D.Get();
				ts3DebugAssert( d3d11Texture3D );


				D3D11_TEXTURE3D_DESC texture3DDesc;
				d3d11Texture3D->GetDesc( &texture3DDesc );

				d3d11RTVDesc.Format = texture3DDesc.Format;
				d3d11RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
				d3d11RTVDesc.Texture3D.MipSlice = targetTextureRef.getRefSubResource().uSubRes3D.mipLevel;
				d3d11RTVDesc.Texture3D.FirstWSlice = targetTextureRef.getRefSubResource().uSubRes3D.depthLayerIndex;
				d3d11RTVDesc.Texture3D.MipSlice = 1;

				d3d11Resource = d3d11Texture3D;
			}
			else
			{
				auto * d3d11Texture2D = dx11Texture->mD3D11Texture2D.Get();
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

		ComPtr<ID3D11DepthStencilView> createTextureDepthStencilAttachmentView(
				DX11Texture & pDX11Texture,
				const RenderTargetAttachmentBinding & pAttachmentBinding )
		{
		}

	}
	
}
