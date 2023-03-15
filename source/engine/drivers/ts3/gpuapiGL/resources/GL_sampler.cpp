
#include "GL_sampler.h"
#include "../GL_apiTranslationLayer.h"
#include "../GL_gpuDevice.h"

namespace ts3::gpuapi
{

	GLSampler::GLSampler( GLGPUDevice & pGPUDevice, GLSamplerObjectHandle pGLSamplerObject )
	: Sampler( pGPUDevice )
	, mGLSamplerObject( std::move( pGLSamplerObject ) )
	{ }

	GLSampler::~GLSampler() = default;

	GLSamplerHandle GLSampler::createSampler( GLGPUDevice & pGPUDevice, const SamplerCreateInfo & pCreateInfo )
	{
		GLSamplerState openglSamplerState;
		if( !translateSamplerConfig( pCreateInfo.samplerConfig, openglSamplerState ) )
		{
			return nullptr;
		}

		auto openglSamplerObject = GLSamplerObject::create( openglSamplerState );
		if( !openglSamplerObject )
		{
			return nullptr;
		}

		auto sampler = createGPUAPIObject<GLSampler>( pGPUDevice, std::move( openglSamplerObject ) );

		return sampler;
	}

	bool GLSampler::translateSamplerConfig( const SamplerConfig & pSamplerConfig, GLSamplerState & pOutSamplerState )
	{
		pOutSamplerState.borderColor = pSamplerConfig.borderColor;
		pOutSamplerState.mipLODRange.first = pSamplerConfig.mipLODRange.begin;
		pOutSamplerState.mipLODRange.second = pSamplerConfig.mipLODRange.end;

		pOutSamplerState.addressModeS = atl::translateGLTextureAddressMode( pSamplerConfig.addressModeConfig.coordU );
		pOutSamplerState.addressModeT = atl::translateGLTextureAddressMode( pSamplerConfig.addressModeConfig.coordV );
		pOutSamplerState.addressModeR = atl::translateGLTextureAddressMode( pSamplerConfig.addressModeConfig.coordW );

		pOutSamplerState.magFilter = atl::chooseGLTextureMagFilter( pSamplerConfig.filterConfig.magFilter, pSamplerConfig.filterConfig.mipMode );
		if( pOutSamplerState.magFilter == GL_INVALID_VALUE )
		{
			return false;
		}

		pOutSamplerState.minFilter = atl::chooseGLTextureMinFilter( pSamplerConfig.filterConfig.minFilter, pSamplerConfig.filterConfig.mipMode );
		if( pOutSamplerState.minFilter == GL_INVALID_VALUE )
		{
			return false;
		}

	#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_ANISOTROPIC_FILTER )
		pOutSamplerState.anisotropyLevel = pSamplerConfig.filterConfig.anisotropyLevel;
		if( pOutSamplerState.anisotropyLevel > 0 )
		{
			GLint maxAnisotropy = 0;

			glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy );
			ts3OpenGLCheckLastResult();

			if( pOutSamplerState.anisotropyLevel > maxAnisotropy )
			{
				pOutSamplerState.anisotropyLevel = maxAnisotropy;
			}
		}
	#endif

		if( pSamplerConfig.textureCompareMode == ETextureCompareMode::RefToTexture )
		{
			pOutSamplerState.textureCompareMode = GL_COMPARE_REF_TO_TEXTURE;
			pOutSamplerState.textureCompareFunc = atl::translateGLCompFunc( pSamplerConfig.textureCompareFunc );
		}
		else
		{
			pOutSamplerState.textureCompareMode = GL_NONE;
			pOutSamplerState.textureCompareFunc = GL_INVALID_VALUE;
		}

		return true;
	}

} // namespace ts3::gpuapi
