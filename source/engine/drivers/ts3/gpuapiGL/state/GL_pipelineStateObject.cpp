
#include "GL_pipelineStateObject.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/objects/GL_vertexArrayObject.h>
#include <ts3/gpuapiGL/resources/GL_shader.h>
#include <ts3/gpuapi/resources/shader.h>

namespace ts3
{
namespace gpuapi
{

	GLGraphicsPipelineStateObject::GLGraphicsPipelineStateObject( GLGPUDevice & pGPUDevice,
	                                                              RenderTargetLayout pRenderTargetLayout,
	                                                              GraphicsShaderBinding pShaderBinding,
	                                                              ShaderInputSignature pShaderInputSignature,
	                                                              const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet,
	                                                              GLShaderPipelineObjectHandle pGLShaderPipelineObject,
	                                                              GLVertexArrayObjectHandle pGLVertexArrayObject )
	: SeparableGraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderBinding ), std::move( pShaderInputSignature ), pSeparableDescriptorSet )
	, mGLShaderPipelineObject ( std::move( pGLShaderPipelineObject ) )
	, mGLVertexArrayObject ( std::move( pGLVertexArrayObject ) )
	{}

	GLGraphicsPipelineStateObject::~GLGraphicsPipelineStateObject() = default;

	Handle<GLGraphicsPipelineStateObject> GLGraphicsPipelineStateObject::create( GLGPUDevice & pGPUDevice,
	                                                                             const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		CommonPSOState commonPSOState;
		if( !createCommonPSOState( pCreateInfo, commonPSOState ) )
		{
			return nullptr;
		}

		GraphicsPipelineStateDescriptorSet graphicsDescriptorSet;
		graphicsDescriptorSet.blendDescriptorID = pGPUDevice.createBlendDescriptor( pCreateInfo.blendDesc );
		graphicsDescriptorSet.depthStencilDescriptorID = pGPUDevice.createDepthStencilDescriptor( pCreateInfo.depthStencilDesc );
		graphicsDescriptorSet.rasterizerDescriptorID = pGPUDevice.createRasterizerDescriptor( pCreateInfo.rasterizerDesc );
		graphicsDescriptorSet.vertexInputFormatDescriptorID = pGPUDevice.createVertexInputFormatDescriptor( pCreateInfo.vertexInputFormatDesc );

		const auto & vertexInputFormatDescriptor = pGPUDevice.getVertexInputFormatDescriptor( graphicsDescriptorSet.vertexInputFormatDescriptorID );

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
		                                                                                        graphicsDescriptorSet,
		                                                                                        std::move( shaderPipelineObject ),
		                                                                                        std::move( vertexArrayObject ) );
		
		return pipelineStateObject;
	}

	GLShaderPipelineObjectHandle GLGraphicsPipelineStateObject::createGraphicsShaderPipelineObject( const GraphicsShaderBinding & pShaderBinding )
	{
		auto shaderPipelineObject = GLShaderPipelineObject::create();
		ts3DebugAssert( shaderPipelineObject );

		glBindProgramPipeline( shaderPipelineObject->mGLHandle );
		ts3GLHandleLastError();

		for( auto & activeStage : pShaderBinding.activeStageList )
		{
			auto * openglShader = activeStage.shaderObject->queryInterface<GLShader>();
			ts3DebugAssert( openglShader->mGLShaderProgramObject );
			shaderPipelineObject->attachProgram( *( openglShader->mGLShaderProgramObject ) );
		}

		glBindProgramPipeline( 0 );
		ts3GLHandleLastError();

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
		ts3GLHandleLastError();

		for( uint32 attributeCounter = 0; attributeCounter < pVertexInputFormatDescriptor.inputFormatDesc.activeVertexAttributesNum; ++attributeCounter )
		{
			const auto & vertexAttribute = pVertexInputFormatDescriptor.inputFormatDesc.vertexAttributeArray[attributeCounter];
			ts3DebugAssert( vertexAttribute );

			glEnableVertexAttribArray( vertexAttribute.attributeIndex );
			ts3GLHandleLastError();

			glVertexAttribFormat( vertexAttribute.attributeIndex, vertexAttribute.componentsNum, vertexAttribute.baseType, vertexAttribute.normalized, vertexAttribute.relativeOffset );
			ts3GLHandleLastError();

			glVertexAttribDivisor( vertexAttribute.attributeIndex, vertexAttribute.instanceRate );
			ts3GLHandleLastError();

			// This must be called *AFTER* glEnableVertexAttribArray and glVertexAttribFormat. TODO: documentation?
			// Moving this up causes crash during an actual draw call on at least Quadro T2000 and RX580 (Win 10).
			glVertexAttribBinding( vertexAttribute.attributeIndex, vertexAttribute.streamIndex );
			ts3GLHandleLastError();
		}

		glBindVertexArray( 0 );
		ts3GLHandleLastError();

		return vertexArrayObject;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
