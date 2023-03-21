
#include "GL_pipelineStateController.h"
#include "GL_commonGraphicsConfig.h"
#include "GL_inputAssembler.h"
#include "GL_graphicsShaderState.h"
#include "GL_renderTarget.h"
#include "GL_pipelineStateObject.h"
#include "GL_vertexArrayObjectCache.h"
#include "../objects/GL_framebufferObject.h"
#include "../objects/GL_shaderPipelineObject.h"
#include "../objects/GL_vertexArrayObject.h"
#include "../resources/GL_gpuBuffer.h"
#include "../resources/GL_sampler.h"
#include "../resources/GL_shader.h"
#include "../resources/GL_texture.h"

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
			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_STATES_ALL ) )
			{
				const auto * glcGraphicsPSO = _currentCommonState.graphicsPSO->queryInterface<GLGraphicsPipelineStateObject>();
				const auto commonPSOUpdateMask = applyCommonGraphicsConfigState( *glcGraphicsPSO );
				executedUpdatesMask.set( commonPSOUpdateMask );
			}
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT ) )
		{
			const auto & currentRenderTargetBindingInfo = getCurrentRenderTargetBindingInfo();
			applyGLRenderTargetBinding( currentRenderTargetBindingInfo );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_COMMON_RENDER_TARGET_BINDING_BIT );
		}

		applyGraphicsPipelineDynamicState( getRenderPassDynamicState() );

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
					smutil::translateIAVertexStreamDefinitionGL( pIAVertexStreamState.getVertexStreamDefinition() );

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

	Bitmask<uint32> GLGraphicsPipelineStateController::applyCommonGraphicsConfigState( const GLGraphicsPipelineStateObject & pGraphicsPSO )
	{
		Bitmask<uint32> executedUpdatesMask = 0;

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT ) )
		{
			const auto & blendState = pGraphicsPSO.getBlendState();
			_globalStateCache.applyBlendState( blendState.mGLBlendConfig );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_BLEND_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT ) )
		{
			const auto & depthStencilState = pGraphicsPSO.getDepthStencilState();
			const auto & dynamicState = getRenderPassDynamicState();

			if( dynamicState.activeStateMask.isSet( E_GRAPHICS_PIPELINE_DYNAMIC_STATE_FLAG_BLEND_CONSTANT_COLOR_BIT ) )
			{
				_globalStateCache.applyDepthStencilState( depthStencilState.mGLDepthStencilConfig, dynamicState.stencilTestRefValue );
			}
			else
			{
				_globalStateCache.applyDepthStencilState( depthStencilState.mGLDepthStencilConfig, 0x00 );
			}

			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_DEPTH_STENCIL_BIT );
		}

		if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT ) )
		{
			const auto & rasterizerState = pGraphicsPSO.getRasterizerState();
			_globalStateCache.applyRasterizerState( rasterizerState.mGLRasterizerConfig );
			executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_RASTERIZER_BIT );
		}

		return executedUpdatesMask;
	}

	void GLGraphicsPipelineStateController::applyGraphicsPipelineDynamicState( const GraphicsPipelineDynamicState & pDynamicState )
	{
		if( pDynamicState.activeStateMask.isSet( E_GRAPHICS_PIPELINE_DYNAMIC_STATE_FLAG_BLEND_CONSTANT_COLOR_BIT ) )
		{

			glBlendColor(
					pDynamicState.blendConstantColor.fpRed,
					pDynamicState.blendConstantColor.fpGreen,
					pDynamicState.blendConstantColor.fpBlue,
					pDynamicState.blendConstantColor.fpAlpha );
			ts3OpenGLHandleLastError();
		}
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
				_globalStateCache.applyShaderPipelineBinding( shaderLinkageState->mGLShaderPipelineObject->mGLHandle );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
			}

			if( _stateUpdateMask.isSet( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT ) )
			{
				const auto * inputLayoutState = glcGraphicsPSO->getIAInputLayoutState().queryInterface<GLIAInputLayoutImmutableStateCore>();
				_globalStateCache.applyVertexArrayObjectBinding( inputLayoutState->mVertexArrayObject->mGLHandle );
				_currentDrawTopologyProperties.primitiveTopology = inputLayoutState->mGLPrimitiveTopology;
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_IA_INPUT_LAYOUT_BIT );
			}

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER ) )
			{
				const auto & currentIAVertexStreamDefinition = getCurrentIAVertexStreamDefinition();
				applyGLIAVertexBufferBindings( currentIAVertexStreamDefinition.vertexBufferBindings );
				_globalStateCache.applyIndexBufferBinding( currentIAVertexStreamDefinition.indexBufferBinding.handle );
				_currentDrawTopologyProperties.indexBufferBaseOffset = currentIAVertexStreamDefinition.indexBufferBinding.offset;
				_currentDrawTopologyProperties.indexBufferDataType = currentIAVertexStreamDefinition.indexBufferBinding.format;
				_currentDrawTopologyProperties.indexBufferElementByteSize = currentIAVertexStreamDefinition.indexBufferBinding.elementByteSize;
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

		smutil::updateUniformDataCurrentGL(
				*shaderStateCore->mGLShaderPipelineObject,
				pConstantInfo.iStageIndex,
				constantBaseType,
				constantLength,
				pConstantData );
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

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT ) )
			{
				const auto * shaderLinkageState = glcGraphicsPSO->getGraphicsShaderLinkageState().queryInterface<GLGraphicsShaderLinkageImmutableStateCompat>();
				_globalStateCache.applyShaderProgramBinding( shaderLinkageState->mGLShaderProgramObject->mGLHandle );
				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_FLAG_SEPARABLE_STATE_SHADER_LINKAGE_BIT );
			}

			if( _stateUpdateMask.isSetAnyOf( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER ) )
			{
				if( !isIAVertexStreamStateDynamic() )
				{
					const auto * inputLayoutState = glcGraphicsPSO->getIAInputLayoutState().queryInterface<GLIAInputLayoutImmutableStateCompat>();
					const auto * vertexStreamState = _currentCommonState.iaVertexStreamState->queryInterface<GLIAVertexStreamImmutableState>();

					const auto & vertexArrayObject = getCachedVertexArrayObject( *inputLayoutState, *vertexStreamState );

					_globalStateCache.applyVertexArrayObjectBinding( vertexArrayObject.mGLHandle );
					_currentDrawTopologyProperties.primitiveTopology = inputLayoutState->mGLInputLayoutDefinition.primitiveTopology;

					_globalStateCache.applyIndexBufferBinding( vertexStreamState->mGLVertexStreamDefinition.indexBufferBinding.handle );
					_currentDrawTopologyProperties.indexBufferBaseOffset = vertexStreamState->mGLVertexStreamDefinition.indexBufferBinding.offset;
					_currentDrawTopologyProperties.indexBufferDataType = vertexStreamState->mGLVertexStreamDefinition.indexBufferBinding.format;
					_currentDrawTopologyProperties.indexBufferElementByteSize = vertexStreamState->mGLVertexStreamDefinition.indexBufferBinding.elementByteSize;

				}
				else
				{
					const auto * inputLayoutState = glcGraphicsPSO->getIAInputLayoutState().queryInterface<GLIAInputLayoutImmutableStateCompat>();
					const auto & currentVertexStreamDefinition = _dynamicIAVertexStreamDefinition;

					const auto & vertexArrayObject = getCachedVertexArrayObject( inputLayoutState->mGLInputLayoutDefinition, currentVertexStreamDefinition );

					_globalStateCache.applyVertexArrayObjectBinding( vertexArrayObject.mGLHandle );
					_currentDrawTopologyProperties.primitiveTopology = inputLayoutState->mGLInputLayoutDefinition.primitiveTopology;

					_globalStateCache.applyIndexBufferBinding( currentVertexStreamDefinition.indexBufferBinding.handle );
					_currentDrawTopologyProperties.indexBufferBaseOffset = currentVertexStreamDefinition.indexBufferBinding.offset;
					_currentDrawTopologyProperties.indexBufferDataType = currentVertexStreamDefinition.indexBufferBinding.format;
					_currentDrawTopologyProperties.indexBufferElementByteSize = currentVertexStreamDefinition.indexBufferBinding.elementByteSize;
				}

				executedUpdatesMask.set( E_GRAPHICS_STATE_UPDATE_MASK_COMBINED_INPUT_ASSEMBLER );
			}

		}

		_stateUpdateMask.unset( E_GRAPHICS_STATE_UPDATE_MASK_COMMON_ALL | E_GRAPHICS_STATE_UPDATE_MASK_SEPARABLE_ALL );

		return !executedUpdatesMask.empty();
	}

	void GLGraphicsPipelineStateControllerCompat::updateShaderInputInlineConstantData(
			const GLGraphicsShaderLinkageImmutableState & pShaderState,
			const ShaderInputParameterConstant & pConstantInfo,
			const void * pConstantData )
	{
		auto * shaderStateCompat = pShaderState.queryInterface<GLGraphicsShaderLinkageImmutableStateCompat>();

		auto constantBaseType = cxdefs::getVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = cxdefs::getVertexAttribFormatLength( pConstantInfo.iFormat );

		smutil::updateUniformDataExplicitGL(
				*shaderStateCompat->mGLShaderProgramObject,
				pConstantInfo.iStageIndex,
				constantBaseType,
				constantLength,
				pConstantData );
	}

	const GLVertexArrayObject & GLGraphicsPipelineStateControllerCompat::getCachedVertexArrayObject(
			const GLIAInputLayoutImmutableStateCompat & pInputLayoutState,
			const GLIAVertexStreamImmutableState & pVertexStreamState )
	{
		return _vaoCache.getOrCreate( pInputLayoutState, pVertexStreamState );
	}

	const GLVertexArrayObject & GLGraphicsPipelineStateControllerCompat::getCachedVertexArrayObject(
			const GLIAInputLayoutDefinition & pInputLayoutDefinition,
			const GLIAVertexStreamDefinition & pVertexStreamDefinition )
	{
		return _vaoCache.getOrCreate( pInputLayoutDefinition, pVertexStreamDefinition );
	}

} // namespace ts3::gpuapi
