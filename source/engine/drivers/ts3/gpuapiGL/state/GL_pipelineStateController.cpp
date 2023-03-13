
#include "GL_pipelineStateController.h"
#include "GL_pipelineStateObject.h"
#include "../objects/GL_framebufferObject.h"
#include "../objects/GL_shaderPipelineObject.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../resources/GL_gpuBuffer.h"
#include "../resources/GL_sampler.h"
#include "../resources/GL_shader.h"
#include "../resources/GL_texture.h"
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
		if( _currentCommonState.renderTargetBindingState )
		{
			if( _currentCommonState.renderTargetBindingState->isDynamicOverrideState() )
			{
				return smutil::getGLRenderTargetBindingInfo( _dynamicRenderTargetBindingDefinition );
			}
			else
			{
				const auto * glcRTBindingState = _currentCommonState.renderTargetBindingState->getInterface<GLRenderTargetBindingImmutableState>();
				return glcRTBindingState->getGLRenderTargetBindingInfo();
			}
		}
		else
		{
			return GLRenderTargetBindingInfo{};
		}
		return glcRTBindingInfo;
	}

	bool GLGraphicsPipelineStateController::applyStateChanges()
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_PSO_BIT ) )
		{
			const auto * glcGraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL ) )
			{
				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT ) )
				{
					const auto & blendState = glcGraphicsPSO->getBlendState();
					applyGLBlendState( blendState );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT ) )
				{
					const auto & depthStencilState = glcGraphicsPSO->getDepthStencilState();
					applyGLDepthStencilState( depthStencilState );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT ) )
				{
					const auto & rasterizerState = glcGraphicsPSO->getRasterizerState();
					applyGLRasterizerState( rasterizerState );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT );
				}

				if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
				{
					const auto & shaderLinkageState = glcGraphicsPSO->getGraphicsShaderLinkageState();
					applyGLShaderLinkageState( shaderLinkageState );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT ) )
				{
					const auto & inputLayoutState = glcGraphicsPSO->getIAInputLayoutState();
					applyGLIAInputLayoutState( inputLayoutState, _currentDrawTopologyProperties );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT );
				}
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
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			const auto & currentRenderTargetBindingInfo = getCurrentRenderTargetBindingInfo();
			applyGLRenderTargetBinding( currentRenderTargetBindingInfo );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		}

		_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL | E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL );

		return !executedUpdatesMask.empty();
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

	bool GLGraphicsPipelineStateController::setBlendConstantColor( const math::RGBAColorR32Norm & pColor )
	{
		bool baseResult = GraphicsPipelineStateController::setBlendConstantColor( pColor );

		if( baseResult )
		{
			glBlendColor( pColor.fpRed, pColor.fpGreen, pColor.fpBlue, pColor.fpAlpha );
			ts3OpenGLHandleLastError();
		}

		return baseResult;
	}

	bool GLGraphicsPipelineStateController::setViewport( const ViewportDesc & pViewportDesc )
	{
		bool baseResult = GraphicsPipelineStateController::setViewport( pViewportDesc );

		if( baseResult )
		{
			glViewport(
				numeric_cast<GLsizei>( pViewportDesc.origin.x ),
		        numeric_cast<GLsizei>( pViewportDesc.origin.y ),
	        numeric_cast<GLsizei>( pViewportDesc.size.x ),
	        numeric_cast<GLsizei>( pViewportDesc.size.y ) );
			ts3OpenGLHandleLastError();

			glDepthRangef( pViewportDesc.depthRange.zNear, pViewportDesc.depthRange.zFar );
			ts3OpenGLHandleLastError();
		}

		return baseResult;
	}

	bool GLGraphicsPipelineStateController::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		bool baseResult = GraphicsPipelineStateController::setShaderConstant( pParamRefID, pData );

		if( baseResult )
		{
			const auto & glcGraphicsPSO = getCurrentGraphicsPSORef<GLGraphicsPipelineStateObject>();
			if( const auto & inputSignature = glcGraphicsPSO.mShaderInputSignature )
			{
				const auto & constantInfo = inputSignature.getConstantInfo( pParamRefID );
				if( constantInfo.iVisibilityMask != 0 )
				{
					const auto * shaderLinkageState = getCurrentSeparableStates().shaderLinkageState;
					const auto * glcShaderLinkageState = shaderLinkageState->queryInterface<GLGraphicsShaderLinkageImmutableState>();

					updateShaderInputInlineConstantData( *glcShaderLinkageState, constantInfo, pData );

					return true;
				}
			}
			return false;
		}

		return baseResult;
	}

	bool GLGraphicsPipelineStateController::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		bool baseResult = GraphicsPipelineStateController::setShaderConstantBuffer( pParamRefID, pConstantBuffer );

		if( baseResult )
		{
			const auto & glcGraphicsPSO = getCurrentGraphicsPSORef<GLGraphicsPipelineStateObject>();
			if( const auto & inputSignature = glcGraphicsPSO.mShaderInputSignature )
			{
				const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
				ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
				ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::CBVConstantBuffer );

				if( descriptorInfo.dShaderVisibilityMask != 0 )
				{
					auto * openglBuffer = pConstantBuffer.queryInterface<GLGPUBuffer>();

					glBindBufferBase( GL_UNIFORM_BUFFER, descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, openglBuffer->mGLBufferObject->mGLHandle );
					ts3OpenGLHandleLastError();

					return true;
				}
			}
		}

		return baseResult;
	}

	bool GLGraphicsPipelineStateController::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		bool baseResult = GraphicsPipelineStateController::setShaderTextureImage( pParamRefID, pTexture );

		if( baseResult )
		{
			const auto & glcGraphicsPSO = getCurrentGraphicsPSORef<GLGraphicsPipelineStateObject>();
			if( const auto & inputSignature = glcGraphicsPSO.mShaderInputSignature )
			{
				const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
				ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
				ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::SRVTextureImage );

				auto * openglTexture = pTexture.queryInterface<GLTexture>();

				glActiveTexture( GL_TEXTURE0 + descriptorInfo.uResourceInfo.resourceBaseRegisterIndex );
				ts3OpenGLHandleLastError();

				glBindTexture( openglTexture->mGLTextureObject->mGLTextureBindTarget, openglTexture->mGLTextureObject->mGLHandle );
				ts3OpenGLHandleLastError();

				return true;
			}
		}

		return baseResult;
	}

	bool GLGraphicsPipelineStateController::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		bool baseResult = GraphicsPipelineStateController::setShaderTextureSampler( pParamRefID, pSampler );

		if( baseResult )
		{
			const auto & glcGraphicsPSO = getCurrentGraphicsPSORef<GLGraphicsPipelineStateObject>();
			if( const auto & inputSignature = glcGraphicsPSO.mShaderInputSignature )
			{
				const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
				ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Sampler );

				if( descriptorInfo.dShaderVisibilityMask != 0 )
				{
					auto * openglSampler = pSampler.queryInterface<GLSampler>();

					glBindSampler( descriptorInfo.uSamplerInfo.samplerBindingIndex, openglSampler->mGLSamplerObject->mGLHandle );
					ts3OpenGLHandleLastError();

					return true;
				}
			}
		}

		return baseResult;
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
			if( pBlendState.mBlendFlags.isSet( E_BLEND_CONFIG_FLAG_ENABLE_MRT_INDEPENDENT_BLENDING_BIT ) )
			{
				for( uint32 caIndex = 0; cxdefs::isRTColorAttachmentIndexValid( caIndex ); ++caIndex )
				{
					const auto attachmentBit = cxdefs::makeRTAttachmentFlag( caIndex );
					if( pBlendState.mGLBlendConfig.attachmentsMask.isSet( attachmentBit ) )
					{
						glEnablei( GL_BLEND, caIndex );
						ts3OpenGLHandleLastError();

						const auto & blendProps = pBlendState.mGLBlendConfig.attachments[caIndex];

						glBlendFuncSeparatei( caIndex, blendProps.srcColorFactor, blendProps.dstColorFactor, blendProps.srcAlphaFactor, blendProps.dstAlphaFactor );
						ts3OpenGLHandleLastError();

						glBlendEquationSeparatei( caIndex, blendProps.colorEquation, blendProps.alphaEquation );
						ts3OpenGLHandleLastError();
					}
					else
					{
						glDisablei( GL_BLEND, caIndex );
						ts3OpenGLHandleLastError();
					}
				}
			}
			else
			{
				glEnable( GL_BLEND );
				ts3OpenGLHandleLastError();

				const auto & blendProps = pBlendState.mGLBlendConfig.attachments[0];

				glBlendFuncSeparate( blendProps.srcColorFactor, blendProps.dstColorFactor, blendProps.srcAlphaFactor, blendProps.dstAlphaFactor );
				ts3OpenGLHandleLastError();

				glBlendEquationSeparate( blendProps.colorEquation, blendProps.alphaEquation );
				ts3OpenGLHandleLastError();
			}

			if( pBlendState.mGLBlendConfig.flags.isSetAnyOf( E_BLEND_CONFIG_FLAG_SET_FIXED_BLEND_CONSTANTS_BIT ) )
			{
				const auto & blendFactor = pBlendState.mGLBlendConfig.constantColor;

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

	void GLGraphicsPipelineStateController::updateShaderInputInlineConstantData(
			const GLGraphicsShaderLinkageImmutableState & pShaderState,
			const ShaderInputParameterConstant & pConstantInfo,
			const void * pConstantData )
	{
		auto constantBaseType = cxdefs::getVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = cxdefs::getVertexAttribFormatLength( pConstantInfo.iFormat );

		if( pShaderState.mGLShaderPipelineObject )
		{
			smutil::updateUniformDataCurrent(
					*( pShaderState.mGLShaderPipelineObject ),
					pConstantInfo.iStageIndex,
					constantBaseType,
					constantLength,
					pConstantData );
		}
		else
		{
			smutil::updateUniformDataExplicit(
					*( pShaderState.mGLShaderProgramObject ),
					pConstantInfo.iStageIndex,
					constantBaseType,
					constantLength,
					pConstantData );
		}
	}

} // namespace ts3::gpuapi
