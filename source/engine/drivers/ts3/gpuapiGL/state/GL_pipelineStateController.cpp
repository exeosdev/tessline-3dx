
#include "GL_pipelineStateController.h"
#include "GL_pipelineStateObject.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../state/GL_pipelineStateObject.h"
#include "../state/GL_vertexStreamStateObject.h"

namespace ts3::gpuapi
{

	GLGraphicsPipelineStateController::GLGraphicsPipelineStateController( GLGraphicsPipelineStateDescriptorCache & pDescriptorCache )
	: SeparableGraphicsPipelineStateController()
	, _descriptorCache( &pDescriptorCache )
	{}

	GLGraphicsPipelineStateController::~GLGraphicsPipelineStateController() = default;

	bool GLGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject * pGraphicsPipelineSO )
	{
		bool updateResult = SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			const auto * openglGPSO = _pendingStateObjects.graphicsPipelineSO->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				// nothing
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT ) )
			{
				_currentGLPipelineConfig.shaderPipelineObjectHandle = openglGPSO->mGLShaderPipelineObject->mGLHandle;
			}
		}

		return true;
	}

	bool GLGraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject * pVertexStreamSO )
	{
		bool updateResult = SeparableGraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			const auto * openglVSSO = _pendingStateObjects.vertexStreamSO->queryInterface<GLVertexStreamStateObject>();

			const auto & indexBufferBinding = openglVSSO->mGLVertexDataSourceBinding.indexBufferBinding;
			if( indexBufferBinding.active )
			{
				_currentGLPipelineConfig.indexBufferBaseOffset = static_cast<GLuint>( indexBufferBinding.offset );
				_currentGLPipelineConfig.indexBufferDataType = indexBufferBinding.format;
				_currentGLPipelineConfig.indexBufferElementByteSize = indexBufferBinding.elementByteSize;
			}
		}

		return true;
	}

	void GLGraphicsPipelineStateController::applyPendingPipelineStateChange()
	{
		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			const auto & pendingDescriptors = _pendingPipelineConfig.stateDescriptors;

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT ) )
			{
				const auto & descriptor = _descriptorCache->getBlendDescriptor( pendingDescriptors.blendDescriptorID );
				_setBlendState( descriptor );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT ) )
			{
				const auto & descriptor = _descriptorCache->getDepthStencilDescriptor( pendingDescriptors.depthStencilDescriptorID );
				_setDepthStencilState( descriptor );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT ) )
			{
				const auto & descriptor = _descriptorCache->getRasterizerDescriptor( pendingDescriptors.rasterizerDescriptorID );
				_setRasterizerState( descriptor );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				const auto & descriptor = _descriptorCache->getVertexInputFormatDescriptor( pendingDescriptors.vertexInputFormatDescriptorID );
				_setVertexInputFormatState( descriptor );

				_currentGLPipelineConfig.primitiveTopology = descriptor.inputFormatDesc.primitiveTopology;
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT ) )
			{
				glBindProgramPipeline( _currentGLPipelineConfig.shaderPipelineObjectHandle );
				ts3OpenGLHandleLastError();
			}
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			const auto * openglVSSO = _pendingStateObjects.vertexStreamSO->queryInterface<GLVertexStreamStateObject>();

			const auto & indexBufferBinding = openglVSSO->mGLVertexDataSourceBinding.indexBufferBinding;
			const auto & vertexBufferBinding = openglVSSO->mGLVertexDataSourceBinding.vertexBufferBinding;

			if( indexBufferBinding.active )
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferBinding.buffer );
				ts3OpenGLHandleLastError();
			}

		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			for( auto streamIndex : openglVSSO->mGLVertexDataSourceBinding.vertexStreamActiveIndexArray )
			{
				const auto buffer = vertexBufferBinding[streamIndex].buffer;
				const auto offset = vertexBufferBinding[streamIndex].offset;
				const auto stride = vertexBufferBinding[streamIndex].stride;

				glBindVertexBuffer( streamIndex, buffer, offset, stride );
				ts3OpenGLHandleLastError();
			}
		#else
			for( const auto & streamRange : openglVSSO->mGLVertexDataSourceBinding.vertexStreamActiveRangeList )
			{
				const auto * bufferRangePtr = &( vertexBufferBinding.bufferArray[streamRange.firstIndex] );
				const auto * offsetRangePtr = &( vertexBufferBinding.offsetArray[streamRange.firstIndex] );
				const auto * strideRangePtr = &( vertexBufferBinding.strideArray[streamRange.firstIndex] );

				glBindVertexBuffers( streamRange.firstIndex, streamRange.length, bufferRangePtr, offsetRangePtr, strideRangePtr );
				ts3OpenGLHandleLastError();
			}
		#endif
		}

		SeparableGraphicsPipelineStateController::applyPendingPipelineStateChange();
	}

	void GLGraphicsPipelineStateController::_setBlendState( const GLBlendStateDescriptor & pGLBlendSD )
	{
		if( pGLBlendSD.configDesc.blendState == EBlendState::Disabled )
		{
			glDisable( GL_BLEND );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glEnable( GL_BLEND );
			ts3OpenGLHandleLastError();

			auto & blendFactor = pGLBlendSD.configDesc.factor;
			glBlendFuncSeparate( blendFactor.srcColor, blendFactor.dstColor, blendFactor.srcAlpha, blendFactor.dstAlpha );
			ts3OpenGLHandleLastError();

			const auto & blendEquation = pGLBlendSD.configDesc.equation;
			glBlendEquationSeparate( blendEquation.color, blendEquation.alpha );
			ts3OpenGLHandleLastError();

			const auto & blendColor = pGLBlendSD.configDesc.constantFactor;
			glBlendColor( blendColor.fpRed, blendColor.fpGreen, blendColor.fpBlue, blendColor.fpAlpha );
			ts3OpenGLHandleLastError();
		}
	}

	void GLGraphicsPipelineStateController::_setDepthStencilState( const GLDepthStencilStateDescriptor & pGLDepthStencilSD )
	{
		if( pGLDepthStencilSD.configDesc.depthTestState == EDepthTestState::Disabled )
		{
			glDisable( GL_DEPTH_TEST );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glEnable( GL_DEPTH_TEST );
			ts3OpenGLHandleLastError();

			glDepthFunc( pGLDepthStencilSD.configDesc.depthSettings.depthCompFunc );
			ts3OpenGLHandleLastError();

			glDepthMask( pGLDepthStencilSD.configDesc.depthSettings.writeMask ? GL_TRUE : GL_FALSE );
			ts3OpenGLHandleLastError();
		}

		if( pGLDepthStencilSD.configDesc.stencilTestState == EStencilTestState::Disabled )
		{
			glDisable( GL_STENCIL_TEST );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glEnable( GL_STENCIL_TEST );
			ts3OpenGLHandleLastError();
			{
				const auto & frontFace = pGLDepthStencilSD.configDesc.stencilSettings.frontFace;
				glStencilFuncSeparate( GL_FRONT, frontFace.compFunc, frontFace.refValue, frontFace.readMask );
				ts3OpenGLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, frontFace.writeMask );
				ts3OpenGLHandleLastError();

				glStencilOpSeparate( GL_FRONT, frontFace.opFail, frontFace.opPassDepthFail, frontFace.opPassDepthPass );
				ts3OpenGLHandleLastError();
			}
			{
				const auto & backFace = pGLDepthStencilSD.configDesc.stencilSettings.backFace;
				glStencilFuncSeparate( GL_FRONT, backFace.compFunc, backFace.refValue, backFace.readMask );
				ts3OpenGLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, backFace.writeMask );
				ts3OpenGLHandleLastError();

				glStencilOpSeparate( GL_FRONT, backFace.opFail, backFace.opPassDepthFail, backFace.opPassDepthPass );
				ts3OpenGLHandleLastError();
			}
		}
	}

	void GLGraphicsPipelineStateController::_setRasterizerState( const GLRasterizerStateDescriptor & pGLRasterizerSD )
	{
		if( pGLRasterizerSD.configDesc.scissorTestState == EScissorTestState::Disabled )
		{
			glDisable( GL_SCISSOR_TEST );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glEnable( GL_SCISSOR_TEST );
			ts3OpenGLHandleLastError();
		}

		if( pGLRasterizerSD.configDesc.cullMode == 0 )
		{
			glDisable( GL_CULL_FACE );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glEnable( GL_CULL_FACE );
			ts3OpenGLHandleLastError();

			glCullFace( pGLRasterizerSD.configDesc.cullMode );
			ts3OpenGLHandleLastError();
		}

		glFrontFace( pGLRasterizerSD.configDesc.triangleFrontFaceOrder );
		ts3OpenGLHandleLastError();

	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		glPolygonMode( GL_FRONT_AND_BACK, pGLRasterizerSD.configDesc.primitiveFillMode );
		ts3OpenGLHandleLastError();
	#endif
	}
	
	void GLGraphicsPipelineStateController::_setVertexInputFormatState( const GLVertexInputFormatStateDescriptor & pGLVertexInputFormatSD )
	{
		glBindVertexArray( pGLVertexInputFormatSD.vertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();
	}

} // namespace ts3::gpuapi
