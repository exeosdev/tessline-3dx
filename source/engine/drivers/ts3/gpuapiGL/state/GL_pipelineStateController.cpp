
#include "GL_pipelineStateController.h"
#include "GL_pipelineStateObject.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../state/GL_pipelineStateObject.h"
#include "../state/GL_vertexStreamStateObject.h"

namespace ts3
{
namespace gpuapi
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
			auto * openglGraphicsPipelineSO = _csCommonConfig.soGraphicsPipeline->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				auto vertexInputFormatDescriptorID = _csSeparableStateDescriptorSet.vertexInputFormatDescriptorID;
				const auto & vertexInputFormatDescriptor = _descriptorCache->getVertexInputFormatDescriptor( vertexInputFormatDescriptorID );

				_csGLPipelineConfig.primitiveTopology = vertexInputFormatDescriptor.inputFormatDesc.primitiveTopology;
				_csGLPipelineConfig.vertexArrayObjectHandle = openglGraphicsPipelineSO->mGLVertexArrayObject->mGLHandle;
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT ) )
			{
				_csGLPipelineConfig.shaderPipelineObjectHandle = openglGraphicsPipelineSO->mGLShaderPipelineObject->mGLHandle;
			}
		}

		return true;
	}

	bool GLGraphicsPipelineStateController::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		bool updateResult = SeparableGraphicsPipelineStateController::setVertexStreamStateObject( pVertexStreamSO );

		if( updateResult && _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			auto * openglVertexStreamSO = _csCommonConfig.soVertexStream->queryInterface<GLVertexStreamStateObject>();

			const auto & indexBufferBinding = openglVertexStreamSO->mGLVertexDataSourceBinding.indexBufferBinding;
			if( indexBufferBinding.active )
			{
				_csGLPipelineConfig.indexBufferBaseOffset = indexBufferBinding.offset;
				_csGLPipelineConfig.indexBufferDataType = indexBufferBinding.format;
				_csGLPipelineConfig.indexBufferElementByteSize = indexBufferBinding.elementByteSize;
			}
		}

		return true;
	}

	bool GLGraphicsPipelineStateController::updatePipelineState()
	{
		if( !SeparableGraphicsPipelineStateController::updatePipelineState() )
		{
			return false;
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT ) )
		{
			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT ) )
			{
				auto blendDescriptorID = _csSeparableStateDescriptorSet.blendDescriptorID;
				const auto & blendDescriptor = _descriptorCache->getBlendDescriptor( blendDescriptorID );
				_setBlendState( blendDescriptor );
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_BLEND_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT ) )
			{
				auto depthStencilDescriptorID = _csSeparableStateDescriptorSet.depthStencilDescriptorID;
				const auto & depthStencilDescriptor = _descriptorCache->getDepthStencilDescriptor( depthStencilDescriptorID );
				_setDepthStencilState( depthStencilDescriptor );
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_DEPTH_STENCIL_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT ) )
			{
				auto rasterizerDescriptorID = _csSeparableStateDescriptorSet.rasterizerDescriptorID;
				const auto & rasterizerDescriptor = _descriptorCache->getRasterizerDescriptor( rasterizerDescriptorID );
				_setRasterizerState( rasterizerDescriptor );
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_RASTERIZER_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT ) )
			{
				glBindVertexArray( _csGLPipelineConfig.vertexArrayObjectHandle );
				ts3GLHandleLastError();
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_DESCRIPTOR_VERTEX_INPUT_FORMAT_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_SHADER_ANY_STAGE_BIT ) )
			{
				glBindProgramPipeline( _csGLPipelineConfig.shaderPipelineObjectHandle );
				ts3GLHandleLastError();
				_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_SHADER_ALL_MASK );
			}

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_COMMON_SO_VERTEX_STREAM_BIT ) )
		{
			auto * openglVertexStreamSO = _csCommonConfig.soVertexStream->queryInterface<GLVertexStreamStateObject>();

			const auto & indexBufferBinding = openglVertexStreamSO->mGLVertexDataSourceBinding.indexBufferBinding;
			const auto & vertexBufferBinding = openglVertexStreamSO->mGLVertexDataSourceBinding.vertexBufferBinding;

			if( indexBufferBinding.active )
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferBinding.buffer );
				ts3GLHandleLastError();
			}

		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
			for( auto streamIndex : openglVertexStreamSO->mGLVertexDataSourceBinding.vertexStreamActiveIndexArray )
			{
				const auto buffer = vertexBufferBinding[streamIndex].buffer;
				const auto offset = vertexBufferBinding[streamIndex].offset;
				const auto stride = vertexBufferBinding[streamIndex].stride;

				glBindVertexBuffer( streamIndex, buffer, offset, stride );
				ts3GLHandleLastError();
			}
		#else
			for( const auto & streamRange : openglVertexStreamSO->mGLVertexDataSourceBinding.vertexStreamActiveRangeList )
			{
				const auto * bufferRangePtr = &( vertexBufferBinding.bufferArray[streamRange.firstIndex] );
				const auto * offsetRangePtr = &( vertexBufferBinding.offsetArray[streamRange.firstIndex] );
				const auto * strideRangePtr = &( vertexBufferBinding.strideArray[streamRange.firstIndex] );

				glBindVertexBuffers( streamRange.firstIndex, streamRange.length, bufferRangePtr, offsetRangePtr, strideRangePtr );
				ts3GLHandleLastError();
			}
		#endif

			_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_COMMON_SO_GRAPHICS_PIPELINE_BIT );
		}

		return _stateUpdateMask != 0;
	}

	void GLGraphicsPipelineStateController::_setBlendState( const GLBlendStateDescriptor & pGLBlendDescriptor )
	{
		if( pGLBlendDescriptor.configDesc.blendState == EBlendState::Disabled )
		{
			glDisable( GL_BLEND );
			ts3GLHandleLastError();
		}
		else
		{
			glEnable( GL_BLEND );
			ts3GLHandleLastError();

			auto & blendFactor = pGLBlendDescriptor.configDesc.factor;
			glBlendFuncSeparate( blendFactor.srcColor, blendFactor.dstColor, blendFactor.srcAlpha, blendFactor.dstAlpha );
			ts3GLHandleLastError();

			const auto & blendEquation = pGLBlendDescriptor.configDesc.equation;
			glBlendEquationSeparate( blendEquation.color, blendEquation.alpha );
			ts3GLHandleLastError();

			const auto & blendColor = pGLBlendDescriptor.configDesc.constantFactor;
			glBlendColor( blendColor.fpRed, blendColor.fpGreen, blendColor.fpBlue, blendColor.fpAlpha );
			ts3GLHandleLastError();
		}
	}

	void GLGraphicsPipelineStateController::_setDepthStencilState( const GLDepthStencilStateDescriptor & pGLDepthStencilDescriptor )
	{
		if( pGLDepthStencilDescriptor.configDesc.depthTestState == EDepthTestState::Disabled )
		{
			glDisable( GL_DEPTH_TEST );
			ts3GLHandleLastError();
		}
		else
		{
			glEnable( GL_DEPTH_TEST );
			ts3GLHandleLastError();

			glDepthFunc( pGLDepthStencilDescriptor.configDesc.depthSettings.depthCompFunc );
			ts3GLHandleLastError();

			glDepthMask( pGLDepthStencilDescriptor.configDesc.depthSettings.writeMask ? GL_TRUE : GL_FALSE );
			ts3GLHandleLastError();
		}

		if( pGLDepthStencilDescriptor.configDesc.stencilTestState == EStencilTestState::Disabled )
		{
			glDisable( GL_STENCIL_TEST );
			ts3GLHandleLastError();
		}
		else
		{
			glEnable( GL_STENCIL_TEST );
			ts3GLHandleLastError();
			{
				const auto & frontFace = pGLDepthStencilDescriptor.configDesc.stencilSettings.frontFace;
				glStencilFuncSeparate( GL_FRONT, frontFace.compFunc, frontFace.refValue, frontFace.readMask );
				ts3GLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, frontFace.writeMask );
				ts3GLHandleLastError();

				glStencilOpSeparate( GL_FRONT, frontFace.opFail, frontFace.opPassDepthFail, frontFace.opPassDepthPass );
				ts3GLHandleLastError();
			}
			{
				const auto & backFace = pGLDepthStencilDescriptor.configDesc.stencilSettings.backFace;
				glStencilFuncSeparate( GL_FRONT, backFace.compFunc, backFace.refValue, backFace.readMask );
				ts3GLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, backFace.writeMask );
				ts3GLHandleLastError();

				glStencilOpSeparate( GL_FRONT, backFace.opFail, backFace.opPassDepthFail, backFace.opPassDepthPass );
				ts3GLHandleLastError();
			}
		}
	}

	void GLGraphicsPipelineStateController::_setRasterizerState( const GLRasterizerStateDescriptor & pGLStateDescriptor )
	{
		if( pGLStateDescriptor.configDesc.scissorTestState == EScissorTestState::Disabled )
		{
			glDisable( GL_SCISSOR_TEST );
			ts3GLHandleLastError();
		}
		else
		{
			glEnable( GL_SCISSOR_TEST );
			ts3GLHandleLastError();
		}

		if( pGLStateDescriptor.configDesc.cullMode == 0 )
		{
			glDisable( GL_CULL_FACE );
			ts3GLHandleLastError();
		}
		else
		{
			glEnable( GL_CULL_FACE );
			ts3GLHandleLastError();

			glCullFace( pGLStateDescriptor.configDesc.cullMode );
			ts3GLHandleLastError();
		}

		glFrontFace( pGLStateDescriptor.configDesc.triangleFrontFaceOrder );
		ts3GLHandleLastError();

	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		glPolygonMode( GL_FRONT_AND_BACK, pGLStateDescriptor.configDesc.primitiveFillMode );
		ts3GLHandleLastError();
	#endif
	}

} /* namespace ts3 */
} /* namespace gpuapi */
