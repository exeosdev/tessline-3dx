
#include "GL_gpuDevice.h"
#include "GL_gpuDriver.h"
#include "GL_commandSystem.h"
#include "GL_presentationLayer.h"
#include "resources/GL_gpuBuffer.h"
#include "resources/GL_sampler.h"
#include "resources/GL_shader.h"
#include "resources/GL_texture.h"
#include "state/GL_pipelineStateObject.h"
#include "state/GL_renderTargetStateObject.h"
#include "state/GL_vertexStreamStateObject.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/shader.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3
{
namespace gpuapi
{

	GLGPUDevice::GLGPUDevice( GLGPUDriver & pGLGPUDriver )
	: GPUDevice( pGLGPUDriver )
	, mSysGLDriver( pGLGPUDriver.mSysGLDriver )
	{}

	GLGPUDevice::~GLGPUDevice() = default;

	GraphicsPipelineStateObjectHandle GLGPUDevice::createGraphicsPipelineStateObject( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		auto graphicsPipelineStateObject = GLGraphicsPipelineStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( graphicsPipelineStateObject );
		return graphicsPipelineStateObject;
	}

	VertexStreamStateObjectHandle GLGPUDevice::createVertexStreamStateObject( const VertexStreamStateObjectCreateInfo & pCreateInfo )
	{
		auto vertexStreamStateObject = GLVertexStreamStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( vertexStreamStateObject );
		return vertexStreamStateObject;
	}

	RenderTargetStateObjectHandle GLGPUDevice::createRenderTargetStateObject( const RenderTargetStateObjectCreateInfo & pCreateInfo )
	{
		auto renderTargetStateObject = GLRenderTargetStateObject::create( *this, pCreateInfo );
		ts3DebugAssert( renderTargetStateObject );
		return renderTargetStateObject;
	}

	void GLGPUDevice::waitForCommandSync( CommandSync & pCommandSync )
	{
		if( pCommandSync )
		{
			auto * openglCommandSyncData = static_cast<GLCommandSyncData *>( pCommandSync.syncData );

			glClientWaitSync( openglCommandSyncData->openglSyncFence, 0, ts3::Limits<GLuint64>::maxValue );
			ts3GLCheckLastResult();

			releaseGLCommandSyncData( pCommandSync.syncData );
			pCommandSync.syncData = nullptr;
		}
	}

	void GLGPUDevice::initializeCommandSystem()
	{
		ts3DebugAssert( !_commandSystem );
		_commandSystem = createGPUAPIObject<GLCommandSystem>( *this );
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

	GLDebugOutput * GLGPUDevice::getDebugOutputInterface() const
	{
		return _glDebugOutput.get();
	}

	const GLBlendStateDescriptor & GLGPUDevice::getBlendDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getBlendDescriptor( pDescriptorID );
	}

	const GLDepthStencilStateDescriptor & GLGPUDevice::getDepthStencilDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getDepthStencilDescriptor( pDescriptorID );
	}

	const GLRasterizerStateDescriptor & GLGPUDevice::getRasterizerDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getRasterizerDescriptor( pDescriptorID );
	}

	const GLVertexInputFormatStateDescriptor & GLGPUDevice::getVertexInputFormatDescriptor( pipeline_state_descriptor_id_t pDescriptorID ) const
	{
		return _descriptorCache.getVertexInputFormatDescriptor( pDescriptorID );
	}

	pipeline_state_descriptor_id_t GLGPUDevice::createBlendDescriptor( const BlendConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createBlendDescriptor( pConfigDesc );
	}

	pipeline_state_descriptor_id_t GLGPUDevice::createDepthStencilDescriptor( const DepthStencilConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createDepthStencilDescriptor( pConfigDesc );
	}

	pipeline_state_descriptor_id_t GLGPUDevice::createRasterizerDescriptor( const RasterizerConfigDesc & pConfigDesc )
	{
		return _descriptorCache.createRasterizerDescriptor( pConfigDesc );
	}

	pipeline_state_descriptor_id_t GLGPUDevice::createVertexInputFormatDescriptor( const VertexInputFormatDesc & pInputFormatDesc )
	{
		return _descriptorCache.createVertexInputFormatDescriptor( pInputFormatDesc );
	}

	GLGraphicsPipelineStateDescriptorCache & GLGPUDevice::getDescriptorCache()
	{
		return _descriptorCache;
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
	    auto openglShader = GLShader::createSeparableStage( *this, pCreateInfo );
	    ts3DebugAssert( openglShader );
	    return openglShader;
	}

	TextureHandle GLGPUDevice::_drvCreateTexture( const TextureCreateInfo & pCreateInfo )
	{
	    auto openglTexture = GLTexture::create( *this, pCreateInfo );
	    ts3DebugAssert( openglTexture );
	    return openglTexture;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
