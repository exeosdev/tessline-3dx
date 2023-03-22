
#include "MTL_sampler.h"
#include "../MTL_gpuDevice.h"
#include "../MTL_apiTranslationLayer.h"

namespace ts3::gpuapi
{

	MetalSampler::MetalSampler( MetalGPUDevice & pGPUDevice, id<MTLSamplerState> pMTLSamplerState )
	: Sampler( pGPUDevice )
	, mMTLSamplerState( pMTLSamplerState )
	{}

	MetalSampler::~MetalSampler() = default;

	MetalSamplerHandle MetalSampler::createSampler( MetalGPUDevice & pGPUDevice, const SamplerCreateInfo & pCreateInfo )
	{
		auto * mtlSamplerDescriptor = translateSamplerConfig( pCreateInfo.samplerConfig );
		auto * mtlSamplerState = [pGPUDevice.mMTLDevice newSamplerStateWithDescriptor:mtlSamplerDescriptor];

		[mtlSamplerDescriptor dealloc];

		auto metalSampler = createGPUAPIObject<MetalSampler>( pGPUDevice, mtlSamplerState );

		return metalSampler;
	}

	MTLSamplerDescriptor * MetalSampler::translateSamplerConfig( const SamplerConfig & pSamplerConfig )
	{
		auto * mtlSamplerDescriptor = [[MTLSamplerDescriptor alloc] init];

		const auto borderColor = atl::selectMTLSamplerBorderColor( pSamplerConfig.borderPredefinedColor, pSamplerConfig.borderColor );
		[mtlSamplerDescriptor setBorderColor:borderColor];

		const auto rAddressMode = atl::translateMTLTextureAddressMode( pSamplerConfig.addressModeConfig.coordU );
		const auto sAddressMode = atl::translateMTLTextureAddressMode( pSamplerConfig.addressModeConfig.coordV );
		const auto tAddressMode = atl::translateMTLTextureAddressMode( pSamplerConfig.addressModeConfig.coordW );

		[mtlSamplerDescriptor setRAddressMode:rAddressMode];
		[mtlSamplerDescriptor setSAddressMode:sAddressMode];
		[mtlSamplerDescriptor setTAddressMode:tAddressMode];

		const auto minFilter = atl::translateMTLTextureFilter( pSamplerConfig.filterConfig.minFilter );
		const auto magFilter = atl::translateMTLTextureFilter( pSamplerConfig.filterConfig.magFilter );
		const auto mipMode = atl::translateMTLTextureMipMode( pSamplerConfig.filterConfig.mipMode );

		[mtlSamplerDescriptor setMinFilter:minFilter];
		[mtlSamplerDescriptor setMagFilter:magFilter];
		[mtlSamplerDescriptor setMipFilter:mipMode];
		[mtlSamplerDescriptor setMaxAnisotropy:pSamplerConfig.filterConfig.anisotropyLevel];

		const auto compFunction = atl::translateMTLCompFunc( pSamplerConfig.textureCompareFunc );
		[mtlSamplerDescriptor setCompareFunction:compFunction];

		[mtlSamplerDescriptor setLodMinClamp:pSamplerConfig.mipLODRange.begin];
		[mtlSamplerDescriptor setLodMaxClamp:pSamplerConfig.mipLODRange.end];

		[mtlSamplerDescriptor setNormalizedCoordinates:TRUE];

		return mtlSamplerDescriptor;
	}

}
