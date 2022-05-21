
#include "GL_sampler.h"
#include "../GL_coreAPIProxy.h"
#include "../GL_gpuDevice.h"

namespace ts3::gpuapi
{

	GLSampler::GLSampler( GLGPUDevice & pGPUDevice, const SamplerDesc & pSamplerDesc, GLSamplerObjectHandle pGLSamplerObject )
	: Sampler( pGPUDevice, pSamplerDesc )
	, mGLSamplerObject( std::move( pGLSamplerObject ) )
	{ }

	GLSampler::~GLSampler() = default;

	GLSamplerHandle GLSampler::createSampler( GLGPUDevice & pGLGPUDevice, const SamplerCreateInfo & pCreateInfo )
	{
		GLSamplerState openglSamplerState;
		if( !translateSamplerDesc( pCreateInfo.samplerDesc, openglSamplerState ) )
		{
			return nullptr;
		}

		auto openglSamplerObject = GLSamplerObject::create( openglSamplerState );
		if( !openglSamplerObject )
		{
			return nullptr;
		}

		auto sampler = createGPUAPIObject<GLSampler>( pGLGPUDevice, pCreateInfo.samplerDesc, std::move( openglSamplerObject ) );

		return sampler;
	}

	bool GLSampler::translateSamplerDesc( const SamplerDesc & pSamplerDesc, GLSamplerState & pOutSamplerState )
	{
		pOutSamplerState.borderColor = pSamplerDesc.borderColor;
		pOutSamplerState.mipLODRange.first = pSamplerDesc.mipLODRange.begin;
		pOutSamplerState.mipLODRange.second = pSamplerDesc.mipLODRange.end;

		pOutSamplerState.addressModeS = GLCoreAPIProxy::translateGLETextureAddressMode( pSamplerDesc.addressModeConfig.coordU );
		pOutSamplerState.addressModeT = GLCoreAPIProxy::translateGLETextureAddressMode( pSamplerDesc.addressModeConfig.coordV );
		pOutSamplerState.addressModeR = GLCoreAPIProxy::translateGLETextureAddressMode( pSamplerDesc.addressModeConfig.coordW );

		pOutSamplerState.magFilter = GLCoreAPIProxy::chooseGLTextureMagFilter( pSamplerDesc.filterConfig.magFilter, pSamplerDesc.filterConfig.mipMode );
		if( pOutSamplerState.magFilter == GL_INVALID_VALUE )
		{
			return false;
		}

		pOutSamplerState.minFilter = GLCoreAPIProxy::chooseGLTextureMinFilter( pSamplerDesc.filterConfig.minFilter, pSamplerDesc.filterConfig.mipMode );
		if( pOutSamplerState.minFilter == GL_INVALID_VALUE )
		{
			return false;
		}

	#if( TS3GX_GL_FEATURE_SUPPORT_TEXTURE_ANISOTROPIC_FILTER )
		pOutSamplerState.anisotropyLevel = pSamplerDesc.filterConfig.anisotropyLevel;
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

		if( pSamplerDesc.textureCompareState == ETextureCompareState::Enabled )
		{
			pOutSamplerState.textureCompareMode = GL_COMPARE_REF_TO_TEXTURE;
			pOutSamplerState.textureCompareFunc = GLCoreAPIProxy::translateGLCompFunc( pSamplerDesc.textureCompareFunc );
		}
		else
		{
			pOutSamplerState.textureCompareMode = GL_NONE;
			pOutSamplerState.textureCompareFunc = GL_INVALID_VALUE;
		}

		return true;
	}

} // namespace ts3::gpuapi
