
#include "DX11_sampler.h"
#include "../DX11_coreAPIProxy.h"
#include "../DX11_gpuDevice.h"

namespace ts3::gpuapi
{

	DX11Sampler::DX11Sampler( DX11GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc, ComPtr<ID3D11SamplerState> pD3D11SamplerState )
	: Sampler( pGPUDevice, pSamplerDesc )
	, mD3D11SamplerState( std::move( pD3D11SamplerState ) )
	{ }

	DX11Sampler::~DX11Sampler() = default;

	DX11SamplerHandle DX11Sampler::create( DX11GPUDevice & pDX11GPUDevice, const SamplerCreateInfo & pCreateInfo )
	{
		D3D11_SAMPLER_DESC d3d11SamplerDesc;
		if( !translateSamplerDesc( pCreateInfo.samplerDesc, d3d11SamplerDesc ) )
		{
			return nullptr;
		}

		ComPtr<ID3D11SamplerState> d3d11SamplerState;
		auto hResult = pDX11GPUDevice.mD3D11Device1->CreateSamplerState( &d3d11SamplerDesc, d3d11SamplerState.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		auto sampler = createGPUAPIObject<DX11Sampler>( pDX11GPUDevice, pCreateInfo.samplerDesc, d3d11SamplerState );

		return sampler;
	}

	bool DX11Sampler::translateSamplerDesc( const SamplerDesc & pSamplerDesc, D3D11_SAMPLER_DESC & pOutD3D11SamplerDesc )
	{
		pOutD3D11SamplerDesc.AddressU = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerDesc.addressModeConfig.coordU );
		pOutD3D11SamplerDesc.AddressV = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerDesc.addressModeConfig.coordV );
		pOutD3D11SamplerDesc.AddressW = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerDesc.addressModeConfig.coordW );
		pOutD3D11SamplerDesc.ComparisonFunc = DX11CoreAPIProxy::translateDX11CompFunc( pSamplerDesc.textureCompareFunc );

		pOutD3D11SamplerDesc.MaxAnisotropy = pSamplerDesc.filterConfig.anisotropyLevel;
		pOutD3D11SamplerDesc.MinLOD = pSamplerDesc.mipLODRange.begin;
		pOutD3D11SamplerDesc.MaxLOD = pSamplerDesc.mipLODRange.end;
		pOutD3D11SamplerDesc.MipLODBias = pSamplerDesc.mipLODBias;

		pOutD3D11SamplerDesc.BorderColor[0] = pSamplerDesc.borderColor.rgbaArray[0];
		pOutD3D11SamplerDesc.BorderColor[1] = pSamplerDesc.borderColor.rgbaArray[1];
		pOutD3D11SamplerDesc.BorderColor[2] = pSamplerDesc.borderColor.rgbaArray[2];
		pOutD3D11SamplerDesc.BorderColor[3] = pSamplerDesc.borderColor.rgbaArray[3];

		pOutD3D11SamplerDesc.Filter = DX11CoreAPIProxy::translateDX11ETextureFilter( pSamplerDesc.filterConfig.magFilter,
		                                                                         pSamplerDesc.filterConfig.minFilter,
		                                                                         pSamplerDesc.filterConfig.mipMode,
		                                                                         pSamplerDesc.filterConfig.anisotropyLevel );

		return true;
	}

} // namespace ts3::gpuapi
