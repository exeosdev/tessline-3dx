
#include "GL_gpuDevice.h"
#include "GL_gpuDriver.h"
#include "GL_commandSystem.h"
#include "GL_presentationLayer.h"
#include "resources/GL_gpuBuffer.h"
#include "resources/GL_sampler.h"
#include "resources/GL_shader.h"
#include "resources/GL_texture.h"
#include "state/GL_pipelineStateObject.h"

namespace ts3::gpuapi
{

	GLGPUDevice::GLGPUDevice( GLGPUDriver & pGLGPUDriver )
	: GPUDevice( pGLGPUDriver )
	, mSysGLDriver( pGLGPUDriver.mSysGLDriver )
	, _immutableStateFactory( *this )
	, _immutableStateCache( _immutableStateFactory )
	{
		setImmutableStateCache( _immutableStateCache );
	}

	GLGPUDevice::~GLGPUDevice() = default;

	GLDebugOutput * GLGPUDevice::getDebugOutputInterface() const
	{
		return _glDebugOutput.get();
	}

	bool GLGPUDevice::initializeGLDebugOutput()
	{
		if( !_glDebugOutput )
		{
			auto openglDebugOutput = GLDebugOutput::createInterface( GLDebugOutputVersion::ARBExt );
			if( openglDebugOutput )
			{
				_glDebugOutput = std::move( openglDebugOutput );
			}
		}
		return _glDebugOutput ? true : false;
	}

	void GLGPUDevice::waitForCommandSync( CommandSync & pCommandSync )
	{
		if( pCommandSync )
		{
			auto * openglCommandSyncData = static_cast<GLCommandSyncData *>( pCommandSync.syncData );

			glClientWaitSync( openglCommandSyncData->openglSyncFence, 0, Limits<GLuint64>::maxValue );
			ts3OpenGLCheckLastResult();

			releaseGLCommandSyncData( pCommandSync.syncData );
			pCommandSync.syncData = nullptr;
		}
	}

	void GLGPUDevice::initializeCommandSystem()
	{
		ts3DebugAssert( !_commandSystem );
		_commandSystem = createGPUAPIObject<GLCommandSystem>( *this );
	}

	bool GLGPUDevice::_drvOnSetPresentationLayer( PresentationLayerHandle pPresentationLayer )
	{
	    auto * openglPresentationLayer = pPresentationLayer->queryInterface<GLPresentationLayer>();
	    if ( !openglPresentationLayer->mSysGLDisplaySurface )
	    {
	        return false;
	    }

	    auto * openglCommandSystem = _commandSystem->queryInterface<GLCommandSystem>();
	    openglCommandSystem->setTargetGLSurface( openglPresentationLayer->mSysGLDisplaySurface );

	    return true;
	}

	GPUBufferHandle GLGPUDevice::_drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo )
	{
	    auto openglBuffer = GLGPUBuffer::create( *this, pCreateInfo );
	    ts3DebugAssert( openglBuffer );
	    return openglBuffer;
	}

	SamplerHandle GLGPUDevice::_drvCreateSampler( const SamplerCreateInfo & pCreateInfo )
	{
	    auto sampler = GLSampler::createSampler( *this, pCreateInfo );
	    ts3DebugAssert( sampler );
	    return sampler;
	}

	ShaderHandle GLGPUDevice::_drvCreateShader( const ShaderCreateInfo & pCreateInfo )
	{
	    auto openglShader = rcutil::createShaderObject( *this, pCreateInfo );
	    ts3DebugAssert( openglShader );
	    return openglShader;
	}

	TextureHandle GLGPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
	    auto openglTexture = GLTexture::create( *this, pCreateInfo );
	    ts3DebugAssert( openglTexture );
	    return openglTexture;
	}

} // namespace ts3::gpuapi
