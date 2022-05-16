
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
	                                                              const SeparableGraphicsStateDescriptorSet & pSstateDescriptors,
	                                                              GLShaderPipelineObjectHandle pGLShaderPipelineObject,
	                                                              GLVertexArrayObjectHandle pGLVertexArrayObject )
	: SeparableGraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderBinding ), std::move( pShaderInputSignature ), pSstateDescriptors )
	, mGLShaderPipelineObject ( std::move( pGLShaderPipelineObject ) )
	, mGLVertexArrayObject ( std::move( pGLVertexArrayObject ) )
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

		const auto & vertexInputFormatDescriptor = pGPUDevice.getVertexInputFormatDescriptor( sstateDescriptors.vertexInputFormatDescriptorID );

		auto vertexArrayObject = createVertexArrayObjectFormatOnly( vertexInputFormatDescriptor );
		auto shaderPipelineObject = createGraphicsShaderPipelineObject( commonPSOState.shaderBinding );
		if( !vertexArrayObject || !shaderPipelineObject )
		{
			return nullptr;
		}

		auto pipelineStateObject = createDynamicInterfaceObject<GLGraphicsPipelineStateObject>( pGPUDevice,
		                                                                                        std::move( commonPSOState.renderTargetLayout ),
		                                                                                        std::move( commonPSOState.shaderBinding ),
		                                                                                        std::move( commonPSOState.shaderInputSignature ),
		                                                                                        sstateDescriptors,
		                                                                                        std::move( shaderPipelineObject ),
		                                                                                        std::move( vertexArrayObject ) );
		
		return pipelineStateObject;
	}

	GLShaderPipelineObjectHandle GLGraphicsPipelineStateObject::createGraphicsShaderPipelineObject( const GraphicsShaderBinding & pShaderBinding )
	{
		auto shaderPipelineObject = GLShaderPipelineObject::create();
		ts3DebugAssert( shaderPipelineObject );

		glBindProgramPipeline( shaderPipelineObject->mGLHandle );
		ts3OpenGLHandleLastError();

		for( auto & activeStage : pShaderBinding.activeStageList )
		{
			auto * openglShader = activeStage.shaderObject->queryInterface<GLShader>();
			ts3DebugAssert( openglShader->mGLShaderProgramObject );
			shaderPipelineObject->attachProgram( *( openglShader->mGLShaderProgramObject ) );
		}

		glBindProgramPipeline( 0 );
		ts3OpenGLHandleLastError();

		return shaderPipelineObject;
	}

	GLShaderProgramObjectHandle GLGraphicsPipelineStateObject::createGraphicsShaderProgramObject( const GraphicsShaderBinding & pShaderBinding )
	{
		auto shaderProgramObject = GLShaderProgramObject::create( GLShaderProgramType::Combined );
		ts3DebugAssert( shaderProgramObject );

		for( auto & activeStage : pShaderBinding.activeStageList )
		{
			auto * openglShader = activeStage.shaderObject->queryInterface<GLShader>();
			ts3DebugAssert( openglShader->mGLShaderObject );
			shaderProgramObject->attachShader( *( openglShader->mGLShaderObject ) );
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

	GLVertexArrayObjectHandle GLGraphicsPipelineStateObject::createVertexArrayObjectFormatOnly( const GLVertexInputFormatStateDescriptor & pVertexInputFormatDescriptor )
	{
		auto vertexArrayObject = GLVertexArrayObject::create();

		glBindVertexArray( vertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();

		for( uint32 attributeCounter = 0; attributeCounter < pVertexInputFormatDescriptor.inputFormatDesc.activeVertexAttributesNum; ++attributeCounter )
		{
			const auto & vertexAttribute = pVertexInputFormatDescriptor.inputFormatDesc.vertexAttributeArray[attributeCounter];
			ts3DebugAssert( vertexAttribute );

			glEnableVertexAttribArray( vertexAttribute.attributeIndex );
			ts3OpenGLHandleLastError();

			glVertexAttribFormat( vertexAttribute.attributeIndex, vertexAttribute.componentsNum, vertexAttribute.baseType, vertexAttribute.normalized, vertexAttribute.relativeOffset );
			ts3OpenGLHandleLastError();

			glVertexAttribDivisor( vertexAttribute.attributeIndex, vertexAttribute.instanceRate );
			ts3OpenGLHandleLastError();

			// This must be called *AFTER* glEnableVertexAttribArray and glVertexAttribFormat. TODO: documentation?
			// Moving this up causes crash during an actual draw call on at least Quadro T2000 and RX580 (Win 10).
			glVertexAttribBinding( vertexAttribute.attributeIndex, vertexAttribute.streamIndex );
			ts3OpenGLHandleLastError();
		}

		glBindVertexArray( 0 );
		ts3OpenGLHandleLastError();

		return vertexArrayObject;
	}

} // namespace ts3::gpuapi
