
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

	bool GLGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		bool updateResult = SeparableGraphicsPipelineStateController::setGraphicsPipelineStateObject( pGraphicsPipelineSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			const auto * openglGPSO = pGraphicsPipelineSO.queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_ANY_BIT ) )
			{
				const auto & stateDescriptors = _currentSeparableStateDescriptors;

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_BLEND_BIT ) )
				{
					const auto & descriptor = _descriptorCache->getBlendDescriptor( stateDescriptors.blendDescriptorID );
					updateGLBlendState( descriptor );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_DEPTH_STENCIL_BIT ) )
				{
					const auto & descriptor = _descriptorCache->getDepthStencilDescriptor( stateDescriptors.depthStencilDescriptorID );
					updateGLDepthStencilState( descriptor );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_RASTERIZER_BIT ) )
				{
					const auto & descriptor = _descriptorCache->getRasterizerDescriptor( stateDescriptors.rasterizerDescriptorID );
					updateGLRasterizerState( descriptor );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
				{
					const auto & descriptor = _descriptorCache->getVertexInputFormatDescriptor( stateDescriptors.vertexInputFormatDescriptorID );
					updateGLVertexInputFormatState( descriptor );
					_currentGLPipelineConfig.primitiveTopology = descriptor.inputFormatDesc.primitiveTopology;
				}
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SEPARABLE_SHADER_ANY_BIT ) )
			{
				const auto & openglPipelineObject = *( openglGPSO->mGLShaderPipelineObject );
				updateGLShaderBinding( openglPipelineObject );
				_currentGLPipelineConfig.shaderPipelineObjectHandle = openglPipelineObject.mGLHandle;
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SEPARABLE_ALL_BITS_MASK );
			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		return updateResult;
	}

	bool GLGraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		return SeparableGraphicsPipelineStateController::resetGraphicsPipelineStateObject();
	}

	bool GLGraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		bool updateResult = SeparableGraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			const auto * openglVSSO = pVertexStreamSO.queryInterface<GLVertexStreamStateObject>();

			const auto & vertexBuffersBindings = openglVSSO->mGLVertexStreamBindingDescriptor.vertexBuffersBindings;
			updateGLVertexBuffersBindings( vertexBuffersBindings );

			const auto & indexBufferBinding = openglVSSO->mGLVertexStreamBindingDescriptor.indexBufferBinding;
			updateGLIndexBufferBinding( indexBufferBinding );

			_currentGLPipelineConfig.indexBufferBaseOffset = static_cast<GLuint>( indexBufferBinding.offset );
			_currentGLPipelineConfig.indexBufferDataType = indexBufferBinding.format;
			_currentGLPipelineConfig.indexBufferElementByteSize = indexBufferBinding.elementByteSize;

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT );
		}

		return true;
	}

	bool GLGraphicsPipelineStateController::resetVertexStreamStateObject()
	{
		return SeparableGraphicsPipelineStateController::resetVertexStreamStateObject();
	}

	void GLGraphicsPipelineStateController::updateGLBlendState( const GLBlendStateDescriptor & pGLBlendSD )
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

	void GLGraphicsPipelineStateController::updateGLDepthStencilState( const GLDepthStencilStateDescriptor & pGLDepthStencilSD )
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

	void GLGraphicsPipelineStateController::updateGLRasterizerState( const GLRasterizerStateDescriptor & pGLRasterizerSD )
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
	
	void GLGraphicsPipelineStateController::updateGLVertexInputFormatState( const GLVertexInputFormatStateDescriptor & pGLVertexInputFormatSD )
	{
		glBindVertexArray( pGLVertexInputFormatSD.vertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateController::updateGLShaderBinding( const GLShaderPipelineObject & pGLShaderPipelineObject )
	{
		glBindProgramPipeline( pGLShaderPipelineObject.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateController::updateGLIndexBufferBinding( const GLIndexBufferBinding & pBinding )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBinding.handle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateController::updateGLVertexBuffersBindings( const GLVertexBuffersBindings & pBinding )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		for( uint32 streamIndex = 0; streamIndex < E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM; ++streamIndex )
		{
			const auto & vbBinding = pBinding.vbBindingArray[streamIndex];

			glBindVertexBuffer( streamIndex, vbBinding.handle, vbBinding.offset, vbBinding.stride );
			ts3OpenGLHandleLastError();
		}
	#else
		// Bind all vertex buffers at once using multi-bind. Note, that this also updates the unused bind slots,
		// to properly have buffer object '0' set as the source VBO (deactivating it). Doing that makes profiling
		// and debugging a lot easier (unused buffers from previous passes did some confusion in few cases).

		glBindVertexBuffers( 0u, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM,
		                     &( pBinding.vbHandleArray[0] ),
		                     &( pBinding.vbOffsetArray[0] ),
		                     &( pBinding.vbStrideArray[0] ) );
		ts3OpenGLHandleLastError();
	#endif
	}

} // namespace ts3::gpuapi
