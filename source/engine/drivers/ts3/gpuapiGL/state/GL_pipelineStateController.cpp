
#include "GL_pipelineStateController.h"
#include "GL_pipelineStateObject.h"
#include "GL_vertexArrayObjectCache.h"
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
		if( isIAVertexStreamStateDynamic() )
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
					_globalStateCache.applyBlendState( blendState.mGLBlendConfig );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT ) )
				{
					const auto & depthStencilState = glcGraphicsPSO->getDepthStencilState();
					_globalStateCache.applyDepthStencilState( depthStencilState.mGLDepthStencilConfig );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT );
				}

				if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT ) )
				{
					const auto & rasterizerState = glcGraphicsPSO->getRasterizerState();
					_globalStateCache.applyRasterizerState( rasterizerState.mGLRasterizerConfig );
					executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT );
				}
			}
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			const auto & currentRenderTargetBindingInfo = getCurrentRenderTargetBindingInfo();
			applyGLRenderTargetBinding( currentRenderTargetBindingInfo );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		}

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

	const GLVertexArrayObject & GLGraphicsPipelineStateController::getCachedVertexArrayObject(
		const GLIAInputLayoutImmutableState & pInputLayoutState,
		const GLIAVertexStreamImmutableState & pVertexStreamState )
	{
		return _vaoCache.getOrCreate( pInputLayoutState, pVertexStreamState );
	}

	void GLGraphicsPipelineStateController::applyGLIAIndexBufferBinding(
			const GLIAIndexBufferBinding & pIndexBufferBinding,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pIndexBufferBinding.handle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.indexBufferBaseOffset = pIndexBufferBinding.offset;
		pDrawTopologyProperties.indexBufferDataType = pIndexBufferBinding.format;
		pDrawTopologyProperties.indexBufferElementByteSize = pIndexBufferBinding.elementByteSize;
	}

	void GLGraphicsPipelineStateController::applyGLRenderTargetBinding( const GLRenderTargetBindingInfo & pGLRenderTargetBinding )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, pGLRenderTargetBinding.renderFBO->mGLHandle );
		ts3OpenGLHandleLastError();
	}



	bool GLGraphicsPipelineStateControllerCore::applyStateChanges()
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( GLGraphicsPipelineStateController::applyStateChanges() )
		{
			const auto * glcGraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
			{
				const auto * shaderLinkageState = glcGraphicsPSO->getGraphicsShaderLinkageState().queryInterface<GLGraphicsShaderLinkageImmutableStateCore>();
				applyGLShaderLinkageState( *shaderLinkageState );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT ) )
			{
				const auto & inputLayoutState = glcGraphicsPSO->getIAInputLayoutState();
				applyGLIAInputLayoutState( inputLayoutState, _currentDrawTopologyProperties );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT );
			}

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER ) )
			{
				const auto & currentIAVertexStreamDefinition = getCurrentIAVertexStreamDefinition();
				applyGLIAVertexStreamState( currentIAVertexStreamDefinition, _currentDrawTopologyProperties );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER );
			}
		}

		_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL | E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL );

		return !executedUpdatesMask.empty();
	}

	void GLGraphicsPipelineStateControllerCore::updateShaderInputInlineConstantData(
			const GLGraphicsShaderLinkageImmutableState & pShaderState,
			const ShaderInputParameterConstant & pConstantInfo,
			const void * pConstantData )
	{
		auto * shaderStateCore = pShaderState.queryInterface<GLGraphicsShaderLinkageImmutableStateCore>();

		auto constantBaseType = cxdefs::getVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = cxdefs::getVertexAttribFormatLength( pConstantInfo.iFormat );

		smutil::updateUniformDataCurrent(
				*shaderStateCore->mGLShaderPipelineObject,
				pConstantInfo.iStageIndex,
				constantBaseType,
				constantLength,
				pConstantData );
	}

	void GLGraphicsPipelineStateControllerCore::applyGLShaderLinkageState(
			const GLGraphicsShaderLinkageImmutableStateCore & pShaderLinkageState )
	{
		glBindProgramPipeline( pShaderLinkageState.mGLShaderPipelineObject->mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateControllerCore::applyGLIAInputLayoutState(
			const GLIAInputLayoutImmutableState & pInputLayoutState,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		glBindVertexArray( pInputLayoutState.mVertexArrayObject->mGLHandle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.primitiveTopology = pInputLayoutState.mGLInputLayoutDefinition.primitiveTopology;
	}

	void GLGraphicsPipelineStateControllerCore::applyGLIAVertexStreamState(
			const GLIAVertexStreamDefinition & pVertexStreamDefinition,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		applyGLIAIndexBufferBinding( pVertexStreamDefinition.indexBufferBinding, pDrawTopologyProperties );
		applyGLIAVertexBufferBindings( pVertexStreamDefinition.vertexBufferBindings );
	}

	void GLGraphicsPipelineStateControllerCore::applyGLIAVertexBufferBindings( const GLIAVertexBuffersBindings & pVertexBufferBindings )
	{
	#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_ES )
		for( const auto & activeVBRange : pVertexBufferBindings.activeRanges )
		{
			for( uint32 entryOffset = 0; entryOffset < activeVBRange.length; ++entryOffset )
			{
				const auto & vbBinding = pVertexBufferBindings.interleavedBindings[activeVBRange.firstIndex + entryOffset];

				glBindVertexBuffer( streamIndex, vbBinding.handle, vbBinding.offset, vbBinding.stride );
				ts3OpenGLHandleLastError();
			}
		}
	#else
		// Bind all vertex buffers at once using multi-bind. Note, that this also updates the unused bind slots,
		// to properly have buffer object '0' set as the source VBO (deactivating it). Doing that makes profiling
		// and debugging a lot easier (unused buffers from previous passes did some confusion in few cases).
		// glBindVertexBuffers(
		// 	0u,
		// 	gpm::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM,
		// 	&( pVertexBufferBindings.separateBindings.handleArray[0] ),
		// 	&( pVertexBufferBindings.separateBindings.offsetArray[0] ),
		// 	&( pVertexBufferBindings.separateBindings.strideArray[0] ) );
		// ts3OpenGLHandleLastError();

		for( const auto & activeVBRange : pVertexBufferBindings.activeRanges )
		{
			glBindVertexBuffers(
				activeVBRange.firstIndex,
				activeVBRange.length,
				&( pVertexBufferBindings.separateBindings.handleArray[activeVBRange.firstIndex] ),
				&( pVertexBufferBindings.separateBindings.offsetArray[activeVBRange.firstIndex] ),
				&( pVertexBufferBindings.separateBindings.strideArray[activeVBRange.firstIndex] ) );
			ts3OpenGLHandleLastError();
		}
	#endif
	}


	bool GLGraphicsPipelineStateControllerCompat::applyStateChanges()
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( GLGraphicsPipelineStateController::applyStateChanges() )
		{
			const auto * glcGraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<GLGraphicsPipelineStateObject>();

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER ) )
			{
				if( !isIAVertexStreamStateDynamic() )
				{
					const auto & inputLayoutState = glcGraphicsPSO->getIAInputLayoutState();
					const auto * vertexStreamState = _currentCommonState.iaVertexStreamState->queryInterface<GLIAVertexStreamImmutableState>();

					const auto & vertexArrayObject = getCachedVertexArrayObject( inputLayoutState, *vertexStreamState );

					applyGLCombinedInputAssemblerState(
						vertexArrayObject,
						inputLayoutState.mGLInputLayoutDefinition,
						vertexStreamState->mGLVertexStreamDefinition,
						_currentDrawTopologyProperties );
				}
				else
				{
					const auto & inputLayoutState = glcGraphicsPSO->getIAInputLayoutState();
					const auto & currentVertexStreamDefinition = _dynamicIAVertexStreamDefinition;

					const auto & vertexArrayObject = getTransientVertexArrayObject( inputLayoutState.mGLInputLayoutDefinition, currentVertexStreamDefinition );

					applyGLCombinedInputAssemblerState(
						vertexArrayObject,
						inputLayoutState.mGLInputLayoutDefinition,
						currentVertexStreamDefinition,
						_currentDrawTopologyProperties );
				}


				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER );
			}

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
			{
				const auto * shaderLinkageState = glcGraphicsPSO->getGraphicsShaderLinkageState().queryInterface<GLGraphicsShaderLinkageImmutableStateCompat>();
				applyGLShaderLinkageState( *shaderLinkageState );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
			}

		}

		_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL | E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL );

		return !executedUpdatesMask.empty();
	}

	const GLVertexArrayObject & GLGraphicsPipelineStateControllerCompat::getTransientVertexArrayObject(
			const GLIAInputLayoutDefinition & pInputLayoutDefinition,
			const GLIAVertexStreamDefinition & pVertexStreamDefinition )
	{
		if( !_transientVertexArrayObject )
		{
			_transientVertexArrayObject = GLVertexArrayObject::create();
		}

		smutil::updateGLVertexArrayObjectLayoutStreamCombined( *_transientVertexArrayObject, pInputLayoutDefinition, pVertexStreamDefinition );

		return *_transientVertexArrayObject;
	}

	void GLGraphicsPipelineStateControllerCompat::updateShaderInputInlineConstantData(
			const GLGraphicsShaderLinkageImmutableState & pShaderState,
			const ShaderInputParameterConstant & pConstantInfo,
			const void * pConstantData )
	{
		auto * shaderStateCompat = pShaderState.queryInterface<GLGraphicsShaderLinkageImmutableStateCompat>();

		auto constantBaseType = cxdefs::getVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = cxdefs::getVertexAttribFormatLength( pConstantInfo.iFormat );

		smutil::updateUniformDataExplicit(
				*shaderStateCompat->mGLShaderProgramObject,
				pConstantInfo.iStageIndex,
				constantBaseType,
				constantLength,
				pConstantData );
	}

	void GLGraphicsPipelineStateControllerCompat::applyGLShaderLinkageState(
			const GLGraphicsShaderLinkageImmutableStateCompat & pShaderLinkageState )
	{
		glUseProgram( pShaderLinkageState.mGLShaderProgramObject->mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLGraphicsPipelineStateControllerCompat::applyGLCombinedInputAssemblerState(
			const GLVertexArrayObject & pVertexArrayObject,
			const GLIAInputLayoutDefinition & pInputLayoutDefinition,
			const GLIAVertexStreamDefinition & pVertexStreamDefinition,
			GLDrawTopologyProperties & pDrawTopologyProperties )
	{
		glBindVertexArray( pVertexArrayObject.mGLHandle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.primitiveTopology = pInputLayoutDefinition.primitiveTopology;

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pVertexStreamDefinition.indexBufferBinding.handle );
		ts3OpenGLHandleLastError();

		pDrawTopologyProperties.indexBufferBaseOffset = pVertexStreamDefinition.indexBufferBinding.offset;
		pDrawTopologyProperties.indexBufferDataType = pVertexStreamDefinition.indexBufferBinding.format;
		pDrawTopologyProperties.indexBufferElementByteSize = pVertexStreamDefinition.indexBufferBinding.elementByteSize;
	}

} // namespace ts3::gpuapi
