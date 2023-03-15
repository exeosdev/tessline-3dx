
#include "DX11_sampler.h"
#include "../DX11_coreAPIProxy.h"
#include "../DX11_gpuDevice.h"

namespace ts3::gpuapi
{

	DX11Sampler::DX11Sampler( DX11GPUDevice & pGPUDevice, const SamplerConfig & pSamplerConfig, ComPtr<ID3D11SamplerState> pD3D11SamplerState )
	: Sampler( pGPUDevice, pSamplerConfig )
	, mD3D11SamplerState( std::move( pD3D11SamplerState ) )
	{ }

	DX11Sampler::~DX11Sampler() = default;

	DX11SamplerHandle DX11Sampler::create( DX11GPUDevice & pDX11GPUDevice, const SamplerCreateInfo & pCreateInfo )
	{
		D3D11_SAMPLER_DESC d3d11SamplerConfig;
		if( !translateSamplerConfig( pCreateInfo.samplerConfig, d3d11SamplerConfig ) )
		{
			return nullptr;
		}

		ComPtr<ID3D11SamplerState> d3d11SamplerState;
		auto hResult = pDX11GPUDevice.mD3D11Device1->CreateSamplerState( &d3d11SamplerConfig, d3d11SamplerState.GetAddressOf() );

		if( FAILED( hResult ) )
		{
			return nullptr;
		}

		auto sampler = createGPUAPIObject<DX11Sampler>( pDX11GPUDevice, pCreateInfo.samplerConfig, d3d11SamplerState );

		return sampler;
	}

	bool DX11Sampler::translateSamplerConfig( const SamplerConfig & pSamplerConfig, D3D11_SAMPLER_DESC & pOutD3D11SamplerConfig )
	{
		pOutD3D11SamplerConfig.AddressU = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerConfig.addressModeConfig.coordU );
		pOutD3D11SamplerConfig.AddressV = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerConfig.addressModeConfig.coordV );
		pOutD3D11SamplerConfig.AddressW = DX11CoreAPIProxy::translateDX11ETextureAddressMode( pSamplerConfig.addressModeConfig.coordW );
		pOutD3D11SamplerConfig.ComparisonFunc = DX11CoreAPIProxy::translateDX11CompFunc( pSamplerConfig.textureCompareFunc );

		pOutD3D11SamplerConfig.MaxAnisotropy = pSamplerConfig.filterConfig.anisotropyLevel;
		pOutD3D11SamplerConfig.MinLOD = pSamplerConfig.mipLODRange.begin;
		pOutD3D11SamplerConfig.MaxLOD = pSamplerConfig.mipLODRange.end;
		pOutD3D11SamplerConfig.MipLODBias = pSamplerConfig.mipLODBias;

		pOutD3D11SamplerConfig.BorderColor[0] = pSamplerConfig.borderColor.rgbaArray[0];
		pOutD3D11SamplerConfig.BorderColor[1] = pSamplerConfig.borderColor.rgbaArray[1];
		pOutD3D11SamplerConfig.BorderColor[2] = pSamplerConfig.borderColor.rgbaArray[2];
		pOutD3D11SamplerConfig.BorderColor[3] = pSamplerConfig.borderColor.rgbaArray[3];

		pOutD3D11SamplerConfig.Filter = DX11CoreAPIProxy::translateDX11ETextureFilter( pSamplerConfig.filterConfig.magFilter,
		                                                                         pSamplerConfig.filterConfig.minFilter,
		                                                                         pSamplerConfig.filterConfig.mipMode,
		                                                                         pSamplerConfig.filterConfig.anisotropyLevel );

		return true;
	}

} // namespace ts3::gpuapi
