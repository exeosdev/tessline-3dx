
#include "GL_pipelineStateObject.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_vertexArrayObject.h>
#include <ts3/gpuapiGL/resources/GL_shader.h>
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	GLGraphicsPipelineStateObject::GLGraphicsPipelineStateObject( GLGPUDevice & pGPUDevice,
	                                                              RenderTargetLayout pRenderTargetLayout,
	                                                              GraphicsShaderBinding pShaderBinding,
	                                                              ShaderInputSignature pShaderInputSignature,
	                                                              const SeparableGraphicsStateDescriptorSet & pStateDescriptors,
	                                                              GLShaderPipelineObjectHandle pGLShaderPipelineObject )
	: SeparableGraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderBinding ), std::move( pShaderInputSignature ), pStateDescriptors )
	, mGLShaderPipelineObject ( std::move( pGLShaderPipelineObject ) )
	{}

	GLGraphicsPipelineStateObject::~GLGraphicsPipelineStateObject() = default;

	GpaHandle<GLGraphicsPipelineStateObject> GLGraphicsPipelineStateObject::create( GLGPUDevice & pGPUDevice,
	                                                                                const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		CommonPSOState commonPSOState;
		if( !createCommonPSOState( pCreateInfo, commonPSOState ) )
		{
			return nullptr;
		}

		SeparableGraphicsStateDescriptorSet sstateDescriptors;
		sstateDescriptors.blendDescriptorID = pGPUDevice.createBlendDescriptor( pCreateInfo.blendDesc );
		sstateDescriptors.depthStencilDescriptorID = pGPUDevice.createDepthStencilDescriptor( pCreateInfo.depthStencilDesc );
		sstateDescriptors.rasterizerDescriptorID = pGPUDevice.createRasterizerDescriptor( pCreateInfo.rasterizerDesc );
		sstateDescriptors.vertexInputFormatDescriptorID = pGPUDevice.createVertexInputFormatDescriptor( pCreateInfo.vertexInputFormatDesc );

		auto shaderPipelineObject = createGraphicsShaderPipelineObject( commonPSOState.shaderBinding );
		if( !shaderPipelineObject )
		{
			return nullptr;
		}

		auto pipelineStateObject = createDynamicInterfaceObject<GLGraphicsPipelineStateObject>( pGPUDevice,
		                                                                                        std::move( commonPSOState.renderTargetLayout ),
		                                                                                        std::move( commonPSOState.shaderBinding ),
		                                                                                        std::move( commonPSOState.shaderInputSignature ),
		                                                                                        sstateDescriptors,
		                                                                                        std::move( shaderPipelineObject ) );
		
		return pipelineStateObject;
	}

	GLShaderPipelineObjectHandle GLGraphicsPipelineStateObject::createGraphicsShaderPipelineObject( const GraphicsShaderBinding & pShaderBinding )
	{
		auto shaderPipelineObject = GLShaderPipelineObject::create();
		ts3DebugAssert( shaderPipelineObject );

		glBindProgramPipeline( shaderPipelineObject->mGLHandle );
		ts3OpenGLHandleLastError();

		for( auto * shader : pShaderBinding.shaderArray )
		{
			if( shader )
			{
				auto * openglShader = shader->queryInterface<GLShader>();
				ts3DebugAssert( openglShader->mGLShaderProgramObject );
				shaderPipelineObject->attachProgram( *( openglShader->mGLShaderProgramObject ) );
			}
		}

		glBindProgramPipeline( 0 );
		ts3OpenGLHandleLastError();

		return shaderPipelineObject;
	}

	GLShaderProgramObjectHandle GLGraphicsPipelineStateObject::createGraphicsShaderProgramObject( const GraphicsShaderBinding & pShaderBinding )
	{
		auto shaderProgramObject = GLShaderProgramObject::create( GLShaderProgramType::Combined );
		ts3DebugAssert( shaderProgramObject );

		for( auto * shader : pShaderBinding.shaderArray )
		{
			if( shader )
			{
				auto * openglShader = shader->queryInterface<GLShader>();
				ts3DebugAssert( openglShader->mGLShaderProgramObject );
				shaderProgramObject->attachShader( *( openglShader->mGLShaderObject ) );
			}
		}

		if( !shaderProgramObject->link() )
		{
			ts3DebugInterrupt();
			return nullptr;
		}
		if( !shaderProgramObject->validate() )
		{
			ts3DebugInterrupt();
			return nullptr;
		}

		shaderProgramObject->detachAllShaders();

		return shaderProgramObject;
	}

} // namespace ts3::gpuapi
