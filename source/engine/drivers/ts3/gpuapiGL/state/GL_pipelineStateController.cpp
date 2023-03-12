
#include "GL_pipelineStateController.h"
#include "GL_pipelineStateObject.h"
#include "../objects/GL_framebufferObject.h"
#include "../objects/GL_shaderPipelineObject.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../state/GL_pipelineStateObject.h"
#include "../state/GL_commonGraphicsConfig.h"
#include "../state/GL_inputAssembler.h"
#include "../state/GL_graphicsShaderState.h"
#include "../state/GL_renderTarget.h"

#include <ts3/gpuapi/state/inputAssemblerDynamicStates.h>
#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3::gpuapi
{

	GLGraphicsPipelineStateController::GLGraphicsPipelineStateController()
	: GraphicsPipelineStateControllerSeparable()
	{}

	GLGraphicsPipelineStateController::~GLGraphicsPipelineStateController() = default;

	const GLDrawTopologyProperties & GLGraphicsPipelineStateController::getGLDrawTopologyProperties() const noexcept
	{
		return _currentDrawTopologyProperties;
	}

	const GLIAVertexStreamDefinition & GLGraphicsPipelineStateController::getCurrentIAVertexStreamDefinition() const noexcept
	{
		if( _currentCommonState.iaVertexStreamState->isDynamicOverrideState() )
		{
			return _dynamicIAVertexStreamDefinition;
		}
		else
		{
			const auto * glcVertexStreamState = _currentCommonState.iaVertexStreamState->queryInterface<GLIAVertexStreamImmutableState>();
			return glcVertexStreamState->mGLVertexStreamDefinition;
		}
	}

	GLRenderTargetBindingInfo GLGraphicsPipelineStateController::getCurrentRenderTargetBindingInfo() const noexcept
	{
		GLRenderTargetBindingInfo glcRTBindingInfo{};
		if( _currentCommonState.renderTargetBindingState->isDynamicOverrideState() )
		{
			return smutil::getGLRenderTargetBindingInfo( _dynamicRenderTargetBindingDefinition );
		}
		else
		{
			const auto * glcRTBindingState = _currentCommonState.renderTargetBindingState->getInterface<GLRenderTargetBindingImmutableState>();
			return glcRTBindingState->getGLRenderTargetBindingInfo();
		}
		return glcRTBindingInfo;
	}

	void GLGraphicsPipelineStateController::applyPipelineStateChanges()
	{
		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT ) )
		{
			const auto * glcGraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL ) )
			{
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT ) )
				{
					const auto & blendState = glcGraphicsPSO->getBlendState();
					applyGLBlendState( blendState );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT ) )
				{
					const auto & depthStencilState = glcGraphicsPSO->getDepthStencilState();
					applyGLDepthStencilState( depthStencilState );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT ) )
				{
					const auto & rasterizerState = glcGraphicsPSO->getRasterizerState();
					applyGLRasterizerState( rasterizerState );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT ) )
				{
					const auto & inputLayoutState = glcGraphicsPSO->getIAInputLayoutState();
					applyGLIAInputLayoutState( inputLayoutState, _currentDrawTopologyProperties );
				}
			}

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_SHADERS_ALL ) )
			{
				const auto & shaderLinkageState = glcGraphicsPSO->getGraphicsShaderLinkageState();
				applyGLShaderLinkageState( shaderLinkageState );
			}
		}

		// Problem: Vertex Buffers (bindings) are part of the VAO state (combined with Input Layout). Since we use VAOs
		// solely for the IL definition and only one VAO can be active at any given time, we don't have a way to handle
		// this as the GL would want us to.
		// Solution: bind Vertex Buffers whenever ANY of the Input Assembler state gets changed (either the Input Layout
		// or the Vertex Stream). VB bindings will basically overwrite bindings of the currently bound VAO.

		if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER ) )
		{
			const auto & currentIAVertexStreamDefinition = getCurrentIAVertexStreamDefinition();
			applyGLIAVertexStreamState( currentIAVertexStreamDefinition, _currentDrawTopologyProperties );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			const auto & currentRenderTargetBindingInfo = getCurrentRenderTargetBindingInfo();
			applyGLRenderTargetBinding( currentRenderTargetBindingInfo );
		}

		_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL | E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL );
	}

	bool GLGraphicsPipelineStateController::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPSO )
	{
		return GraphicsPipelineStateControllerSeparable::setGraphicsPipelineStateObject( pGraphicsPSO );
	}

	bool GLGraphicsPipelineStateController::resetGraphicsPipelineStateObject()
	{
		return GraphicsPipelineStateControllerSeparable::resetGraphicsPipelineStateObject();
	}

	bool GLGraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamDynamicState & pIAVertexStreamState )
	{
		bool updateResult = GraphicsPipelineStateController::setIAVertexStreamState( pIAVertexStreamState );

		if( updateResult )
		{
			auto glcVertexStreamDefinition =
					smutil::translateIAVertexStreamDefinition( pIAVertexStreamState.getVertexStreamDefinition() );

			_dynamicIAVertexStreamDefinition = glcVertexStreamDefinition;
		}

		return updateResult;
	}

	bool GLGraphicsPipelineStateController::setIAVertexStreamState( const IAVertexStreamImmutableState & pIAVertexStreamState )
	{
		return GraphicsPipelineStateControllerSeparable::setIAVertexStreamState( pIAVertexStreamState );
	}

	bool GLGraphicsPipelineStateController::resetIAVertexStreamState()
	{
		return GraphicsPipelineStateControllerSeparable::resetIAVertexStreamState();
	}

	bool GLGraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingDynamicState & pRenderTargetBindingState )
	{
		bool updateResult = GraphicsPipelineStateController::setRenderTargetBindingState( pRenderTargetBindingState );

		if( updateResult )
		{
			auto glcRenderTargetBindingDefinition =
					smutil::translateRenderTargetBindingDefinition( pRenderTargetBindingState.getBindingDefinition() );

			_dynamicRenderTargetBindingDefinition = std::move( glcRenderTargetBindingDefinition );

		}

		return updateResult;
	}

	bool GLGraphicsPipelineStateController::setRenderTargetBindingState( const RenderTargetBindingImmutableState & pRenderTargetBindingState )
	{
		return GraphicsPipelineStateControllerSeparable::setRenderTargetBindingState( pRenderTargetBindingState );
	}

	bool GLGraphicsPipelineStateController::resetRenderTargetBindingState()
	{
		return GraphicsPipelineStateControllerSeparable::resetRenderTargetBindingState();
	}

	void GLGraphicsPipelineStateController::resetDynamicIAVertexStreamState()
	{}

	void GLGraphicsPipelineStateController::resetDynamicRenderTargetBindingState()
	{
		_dynamicRenderTargetBindingDefinition.fboData.renderFBO.reset();
		_dynamicRenderTargetBindingDefinition.fboData.resolveFBO.reset();
	}

	void GLGraphicsPipelineStateController::applyGLBlendState( const GLBlendImmutableState & pBlendState )
	{
		if( pBlendState.mGLBlendConfig.attachmentsMask.isSetAnyOf( E_RT_ATTACHMENT_MASK_COLOR_ALL ) )
		{
			glEnable( GL_BLEND );
			ts3OpenGLHandleLastError();

			if( pBlendState.mBlendFlags.isSet( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT ) )
			{
				for( uint32 caIndex = 0; caIndex < cxdefs::RT_MAX_COLOR_ATTACHMENTS_NUM; ++caIndex )
				{
					const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
					if( pBlendState.mGLBlendConfig.attachmentsMask.isSet( attachmentBit ) )
					{
						const auto & blendProps = pBlendState.mGLBlendConfig.attachments[caIndex];

						glBlendFuncSeparate( blendProps.srcColorFactor, blendProps.dstColorFactor, blendProps.srcAlphaFactor, blendProps.dstAlphaFactor );
						ts3OpenGLHandleLastError();

						glBlendEquationSeparate( blendProps.colorEquation, blendProps.alphaEquation );
						ts3OpenGLHandleLastError();

						const auto & blendFactor = pBlendState.mGLBlendConfig.attachments[caIndex].constantFactor;

						glBlendColor( blendFactor.fpRed, blendFactor.fpGreen, blendFactor.fpBlue, blendFactor.fpAlpha );
						ts3OpenGLHandleLastError();
					}
				}
			}
			else
			{
				const auto & blendProps = pBlendState.mGLBlendConfig.attachments[0];

				glBlendFuncSeparate( blendProps.srcColorFactor, blendProps.dstColorFactor, blendProps.srcAlphaFactor, blendProps.dstAlphaFactor );
				ts3OpenGLHandleLastError();

				glBlendEquationSeparate( blendProps.colorEquation, blendProps.alphaEquation );
				ts3OpenGLHandleLastError();

				const auto & blendFactor = pBlendState.mGLBlendConfig.attachments[0].constantFactor;

				glBlendColor( blendFactor.fpRed, blendFactor.fpGreen, blendFactor.fpBlue, blendFactor.fpAlpha );
				ts3OpenGLHandleLastError();
			}
		}
		else
		{
			glDisable( GL_BLEND );
			ts3OpenGLHandleLastError();
		}
	}

	void GLGraphicsPipelineStateController::applyGLDepthStencilState( const GLDepthStencilImmutableState & pDepthStencilState )
	{
		if( pDepthStencilState.mDepthStencilFlags.isSet( E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_DEPTH_TEST_BIT ) )
		{
			glEnable( GL_DEPTH_TEST );
			ts3OpenGLHandleLastError();

			glDepthFunc( pDepthStencilState.mGLDepthStencilConfig.depthSettings.depthCompFunc );
			ts3OpenGLHandleLastError();

			glDepthMask( pDepthStencilState.mGLDepthStencilConfig.depthSettings.writeMask ? GL_TRUE : GL_FALSE );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
			ts3OpenGLHandleLastError();
		}

		if( pDepthStencilState.mDepthStencilFlags.isSet( E_DEPTH_STENCIL_CONFIG_FLAG_ENABLE_STENCIL_TEST_BIT ) )
		{
			glEnable( GL_STENCIL_TEST );
			ts3OpenGLHandleLastError();
			{
				const auto & frontFace = pDepthStencilState.mGLDepthStencilConfig.stencilSettings.frontFace;
				glStencilFuncSeparate( GL_FRONT, frontFace.compFunc, frontFace.refValue, frontFace.readMask );
				ts3OpenGLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, frontFace.writeMask );
				ts3OpenGLHandleLastError();

				glStencilOpSeparate( GL_FRONT, frontFace.opFail, frontFace.opPassDepthFail, frontFace.opPassDepthPass );
				ts3OpenGLHandleLastError();
			}
			{
				const auto & backFace = pDepthStencilState.mGLDepthStencilConfig.stencilSettings.backFace;
				glStencilFuncSeparate( GL_FRONT, backFace.compFunc, backFace.refValue, backFace.readMask );
				ts3OpenGLHandleLastError();

				glStencilMaskSeparate( GL_FRONT, backFace.writeMask );
				ts3OpenGLHandleLastError();

				glStencilOpSeparate( GL_FRONT, backFace.opFail, backFace.opPassDepthFail, backFace.opPassDepthPass );
				ts3OpenGLHandleLastError();
			}
		}
		else
		{
			glDisable( GL_STENCIL_TEST );
			ts3OpenGLHandleLastError();
		}
	}

	void GLGraphicsPipelineStateController::applyGLRasterizerState( const GLRasterizerImmutableState & pRasterizerState )
	{
		if( pRasterizerState.mGLRasterizerConfig.flags.isSet( E_RASTERIZER_CONFIG_FLAG_ENABLE_SCISSOR_TEST_BIT ) )
		{
			glEnable( GL_SCISSOR_TEST );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_SCISSOR_TEST );
			ts3OpenGLHandleLastError();
		}

		if( pRasterizerState.mGLRasterizerConfig.cullMode != 0 )
		{
			glEnable( GL_CULL_FACE );
			ts3OpenGLHandleLastError();

			glCullFace( pRasterizerState.mGLRasterizerConfig.cullMode );
			ts3OpenGLHandleLastError();
		}
		else
		{
			glDisable( GL_CULL_FACE );
			ts3OpenGLHandleLastError();
		}

		glFrontFace( pRasterizerState.mGLRasterizerConfig.frontFaceVerticesOrder );
		ts3OpenGLHandleLastError();

	#if( TS3GX_GL_FEATURE_SUPPORT_PRIMITIVE_FILL_MODE )
		glPolygonMode( GL_FRONT_AND_BACK, pRasterizerState.mGLRasterizerConfig.primitiveFillMode );
		ts3OpenGLHandleLastError();
	#endif
	}
	
	void GLGraphicsPipelineStateController::applyGLIAInputLayoutState(
			const GLIAInputLayoutImmutableState & pInputLayoutState,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		glBindVertexArray( pInputLayoutState.mVertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.primitiveTopology = pInputLayoutState.mGLInputLayoutDefinition.primitiveTopology;
	}

	void GLGraphicsPipelineStateController::applyGLShaderLinkageState( const GLGraphicsShaderLinkageImmutableState & pShaderLinkageState )
	{
		glBindProgramPipeline( pShaderLinkageState.mGLShaderPipelineObject->mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateController::applyGLIAVertexStreamState(
			const GLIAVertexStreamDefinition & pVertexStreamDefinition,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		applyGLIAVertexStreamStateIBBinding( pVertexStreamDefinition.indexBufferBinding, pDrawTopologyProperties );
		applyGLIAVertexStreamStateVBBindings( pVertexStreamDefinition.vertexBufferBindings );
	}

	void GLGraphicsPipelineStateController::applyGLIAVertexStreamStateIBBinding(
			const GLIAIndexBufferBinding & pIndexBufferBinding,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pIndexBufferBinding.handle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.indexBufferBaseOffset = pIndexBufferBinding.offset;
		pDrawTopologyProperties.indexBufferDataType = pIndexBufferBinding.format;
		pDrawTopologyProperties.indexBufferElementByteSize = pIndexBufferBinding.elementByteSize;
	}

	void GLGraphicsPipelineStateController::applyGLIAVertexStreamStateVBBindings( const GLIAVertexBuffersBindings & pVertexBufferBindings )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		for( uint32 streamIndex = 0; streamIndex < cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM; ++streamIndex )
		{
			const auto & vbBinding = pVertexBufferBindings.interleavedBindings[streamIndex];

			glBindVertexBuffer( streamIndex, vbBinding.handle, vbBinding.offset, vbBinding.stride );
			ts3OpenGLHandleLastError();
		}
	#else
		// Bind all vertex buffers at once using multi-bind. Note, that this also updates the unused bind slots,
		// to properly have buffer object '0' set as the source VBO (deactivating it). Doing that makes profiling
		// and debugging a lot easier (unused buffers from previous passes did some confusion in few cases).

		glBindVertexBuffers(
				0u,
				cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM,
				&( pVertexBufferBindings.separateBindings.handleArray[0] ),
				&( pVertexBufferBindings.separateBindings.offsetArray[0] ),
				&( pVertexBufferBindings.separateBindings.strideArray[0] ) );
		ts3OpenGLHandleLastError();
	#endif
	}

	void GLGraphicsPipelineStateController::applyGLRenderTargetBinding( const GLRenderTargetBindingInfo & pGLRenderTargetBinding )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, pGLRenderTargetBinding.renderFBO->mGLHandle );
		ts3OpenGLHandleLastError();
	}

} // namespace ts3::gpuapi
