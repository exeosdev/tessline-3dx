
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
			auto glcDebugOutput = GLDebugOutput::createInterface( GLDebugOutputVersion::ARBExt );
			if( glcDebugOutput )
			{
				_glDebugOutput = std::move( glcDebugOutput );
			}
		}
		return _glDebugOutput ? true : false;
	}

	void GLGPUDevice::waitForCommandSync( CommandSync & pCommandSync )
	{
		if( pCommandSync )
		{
			auto * glcCommandSyncData = static_cast<GLCommandSyncData *>( pCommandSync.syncData );

			glClientWaitSync( glcCommandSyncData->openglSyncFence, 0, Limits<GLuint64>::maxValue );
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
	    auto * glcPresentationLayer = pPresentationLayer->queryInterface<GLPresentationLayer>();
	    if ( !glcPresentationLayer->mSysGLDisplaySurface )
	    {
	        return false;
	    }

	    auto * glcCommandSystem = _commandSystem->queryInterface<GLCommandSystem>();
	    glcCommandSystem->setTargetGLSurface( glcPresentationLayer->mSysGLDisplaySurface );

	    return true;
	}

	GPUBufferHandle GLGPUDevice::_drvCreateGPUBuffer( const GPUBufferCreateInfo & pCreateInfo )
	{
	    auto glcBuffer = GLGPUBuffer::create( *this, pCreateInfo );
	    ts3DebugAssert( glcBuffer );
	    return glcBuffer;
	}

	SamplerHandle GLGPUDevice::_drvCreateSampler( const SamplerCreateInfo & pCreateInfo )
	{
	    auto glcSampler = GLSampler::createSampler( *this, pCreateInfo );
	    ts3DebugAssert( glcSampler );
	    return glcSampler;
	}

	ShaderHandle GLGPUDevice::_drvCreateShader( const ShaderCreateInfo & pCreateInfo )
	{
	#if( TS3GX_GL_TARGET >= TS3GX_GL_TARGET_GL43 )
		auto glcShader = rcutil::createShaderSeparableStage( *this, pCreateInfo );
	#else
	    auto glcShader = rcutil::createShaderObject( *this, pCreateInfo );
	#endif
	    ts3DebugAssert( glcShader );
	    return glcShader;
	}

	TextureHandle GLGPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
	    auto glcTexture = GLTexture::create( *this, pCreateInfo );
	    ts3DebugAssert( glcTexture );
	    return glcTexture;
	}

	RenderTargetTextureHandle GLGPUDevice::_drvCreateRenderTargetTexture(
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		auto glcRTTexture = GLTexture::createRTT( *this, pCreateInfo );
		ts3DebugAssert( glcRTTexture );
		return glcRTTexture;
	}

	GraphicsPipelineStateObjectHandle GLGPUDevice::_drvCreateGraphicsPipelineStateObject(
			const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		auto glcGraphicsPSO = GLGraphicsPipelineStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( glcGraphicsPSO );
		return glcGraphicsPSO;
	}

} // namespace ts3::gpuapi
