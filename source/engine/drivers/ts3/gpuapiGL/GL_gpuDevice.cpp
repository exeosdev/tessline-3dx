
#include "GL_gpuDevice.h"
#include "GL_gpuDriver.h"
#include "GL_commandSystem.h"
#include "GL_presentationLayer.h"
#include "resources/GL_gpuBuffer.h"
#include "resources/GL_sampler.h"
#include "resources/GL_shader.h"
#include "resources/GL_texture.h"
#include "state/GL_pipelineStateObject.h"
#include "state/GL_pipelineStateController.h"

namespace ts3::gpuapi
{

	GLGPUDevice::GLGPUDevice( GLGPUDriver & pGPUDriver, GLPipelineImmutableStateFactory & pImmutableStateFactory )
	: GPUDevice( pGPUDriver )
	, mSysGLDriver( pGPUDriver.mSysGLDriver )
	, mSysGLSupportInfo( mSysGLDriver->getVersionSupportInfo() )
	, mGLRuntimeSupportFlags( coreutil::queryGLRuntimeSupportFlags( mSysGLSupportInfo ) )
	, _immutableStateFactoryGL( &pImmutableStateFactory )
	, _immutableStateCache( pImmutableStateFactory )
	{
		setImmutableStateCache( _immutableStateCache );
	}

	GLGPUDevice::~GLGPUDevice() = default;

	GLDebugOutput * GLGPUDevice::getDebugOutputInterface() const
	{
		return _glDebugOutput.get();
	}

	RenderTargetBindingImmutableStateHandle GLGPUDevice::createScreenRenderTargetBindingState(
		const RenderTargetLayout & pRenderTargetLayout )
	{
		return _immutableStateFactoryGL->createScreenRenderTargetBindingState( pRenderTargetLayout );
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
	    auto glcBuffer = GLGPUBuffer::createInstance( *this, pCreateInfo );
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
		ShaderHandle glcShader;
		glcShader = GLShader::createInstance( *this, pCreateInfo );
	    return glcShader;
	}

	TextureHandle GLGPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
	    auto glcTexture = GLTexture::createDefault( *this, pCreateInfo );
	    ts3DebugAssert( glcTexture );
	    return glcTexture;
	}

	RenderTargetTextureHandle GLGPUDevice::_drvCreateRenderTargetTexture(
			const RenderTargetTextureCreateInfo & pCreateInfo )
	{
		auto glcRTTexture = GLTexture::createForRenderTarget( *this, pCreateInfo );
		ts3DebugAssert( glcRTTexture );
		return glcRTTexture;
	}

	GraphicsPipelineStateObjectHandle GLGPUDevice::_drvCreateGraphicsPipelineStateObject(
			const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		if( !pCreateInfo.shaderLinkageState )
		{
			pCreateInfo.shaderLinkageState = _immutableStateFactoryGL->createGraphicsShaderLinkageState( pCreateInfo.shaderSet );
		}

		auto glcGraphicsPSO = GLGraphicsPipelineStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( glcGraphicsPSO );

		return glcGraphicsPSO;
	}


	GLGPUDeviceCore::GLGPUDeviceCore( GLGPUDriver & pGPUDriver )
	: GLGPUDevice( pGPUDriver, _immutableStateFactoryCore )
	, _immutableStateFactoryCore( *this )
	{}

	GLGPUDeviceCore::~GLGPUDeviceCore() = default;

	bool GLGPUDeviceCore::isCompatibilityDevice() const noexcept
	{
		return false;
	}


	GLGPUDeviceCompat::GLGPUDeviceCompat( GLGPUDriver & pGPUDriver )
	: GLGPUDevice( pGPUDriver, _immutableStateFactoryCompat )
	, _immutableStateFactoryCompat( *this )
	{}

	GLGPUDeviceCompat::~GLGPUDeviceCompat() = default;

	bool GLGPUDeviceCompat::isCompatibilityDevice() const noexcept
	{
		return true;
	}


} // namespace ts3::gpuapi
