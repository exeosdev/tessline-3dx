
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_SAMPLER_H__
#define __TS3DRIVER_GPUAPI_DX11_SAMPLER_H__

#include "../DX11_prerequisites.h"
#include <ts3/gpuapi/resources/sampler.h>

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( DX11Sampler );

	class DX11Sampler : public Sampler
	{
	public:
		ComPtr<ID3D11SamplerState> const mD3D11SamplerState;

		DX11Sampler( DX11GPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc, ComPtr<ID3D11SamplerState> pD3D11SamplerState );
		virtual ~DX11Sampler();

		static DX11SamplerHandle create( DX11GPUDevice & pDX11GPUDevice, const SamplerCreateInfo & pCreateInfo );

	private:
		static bool translateSamplerDesc( const SamplerDesc & pSamplerDesc, D3D11_SAMPLER_DESC & pOutD3D11SamplerDesc );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_SAMPLER_H__
