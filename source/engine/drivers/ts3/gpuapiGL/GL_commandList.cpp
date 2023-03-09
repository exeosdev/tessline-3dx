
#include "GL_commandList.h"
#include "GL_commandSystem.h"
#include "GL_gpuDevice.h"
#include "objects/GL_framebufferObject.h"
#include "objects/GL_shaderProgramObject.h"
#include "objects/GL_vertexArrayObject.h"
#include "resources/GL_gpuBuffer.h"
#include "resources/GL_sampler.h"
#include "resources/GL_shader.h"
#include "resources/GL_texture.h"
#include "state/GL_pipelineStateObject.h"
#include "state/GL_graphicsShaderState.h"
#include "state/GL_renderTarget.h"

#include <ts3/gpuapi/state/renderTargetDynamicStates.h>

namespace ts3::gpuapi
{

	GLCommandList::GLCommandList(
			GLCommandSystem & pGLCommandSystem,
			ECommandListType pListType,
			system::OpenGLRenderContextHandle pSysGLRenderContext )
	: CommandList( pGLCommandSystem, pListType, _stateController )
	, mSysGLRenderContext( pSysGLRenderContext )
	{}

	GLCommandList::~GLCommandList() = default;

	bool GLCommandList::beginRenderPass( const RenderPassConfigurationImmutableState & pRenderPassState )
	{
		bool beginRenderPassStatus = CommandList::beginRenderPass( pRenderPassState );

		if( beginRenderPassStatus )
		{
			const auto * glcRenderPassState = pRenderPassState.queryInterface<GLRenderPassConfigurationImmutableState>();
			_currentRenderPassConfiguration = glcRenderPassState->mRenderPassConfiguration;

			executeRenderPassLoadActions();
		}

		return true;
	}

	bool GLCommandList::beginRenderPass( const RenderPassConfigurationDynamicState & pRenderPassState )
	{
		bool beginRenderPassStatus = CommandList::beginRenderPass( pRenderPassState );

		if( beginRenderPassStatus )
		{
			_currentRenderPassConfiguration = pRenderPassState.getRenderPassConfiguration();

			executeRenderPassLoadActions();
		}

		return true;
	}

	void GLCommandList::endRenderPass()
	{
		if( !isRenderPassActive() )
		{
			return;
		}

		executeRenderPassStoreActions();

		CommandList::endRenderPass();
	}

	void GLCommandList::beginCommandSequence()
	{
		CommandList::beginCommandSequence();
	}

	void GLCommandList::endCommandSequence()
	{
		CommandList::endCommandSequence();
	}

	void GLCommandList::executeDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugInterrupt();
	}

	void GLCommandList::setViewport( const ViewportDesc & pViewportDesc )
	{
		glViewport( numeric_cast<GLsizei>( pViewportDesc.origin.x ),
		            numeric_cast<GLsizei>( pViewportDesc.origin.y ),
		            numeric_cast<GLsizei>( pViewportDesc.size.x ),
		            numeric_cast<GLsizei>( pViewportDesc.size.y ) );
		ts3OpenGLHandleLastError();

		glDepthRangef( pViewportDesc.depthRange.zNear, pViewportDesc.depthRange.zFar );
		ts3OpenGLHandleLastError();
	}

	bool GLCommandList::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		const auto & openglGPSO = _stateController.getCurrentGraphicsPipelineSORef<GLGraphicsPipelineStateObject>();
		if( const auto & inputSignature = openglGPSO.mShaderInputSignature )
		{
			const auto & constantInfo = inputSignature.getConstantInfo( pParamRefID );
			if( constantInfo.iVisibilityMask != 0 )
			{
				updateShaderInputInlineConstantData( constantInfo, pData );
				return true;
			}
		}
		return false;
	}

	bool GLCommandList::setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer )
	{
		const auto & openglGPSO = _stateController.getCurrentGraphicsPipelineSORef<GLGraphicsPipelineStateObject>();
		if( const auto & inputSignature = openglGPSO.mShaderInputSignature )
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
		return false;
	}

	bool GLCommandList::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		const auto & openglGPSO = _stateController.getCurrentGraphicsPipelineSORef<GLGraphicsPipelineStateObject>();
		if( const auto & inputSignature = openglGPSO.mShaderInputSignature )
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
		return false;
	}

	bool GLCommandList::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		const auto & openglGPSO = _stateController.getCurrentGraphicsPipelineSORef<GLGraphicsPipelineStateObject>();
		if( const auto & inputSignature = openglGPSO.mShaderInputSignature )
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
		return false;
	}

	void GLCommandList::drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
		_stateController.applyPipelineStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();
		const auto * indexDataOffset = reinterpret_cast<void*>( pIndicesOffset * drawTopologyProperties.indexBufferElementByteSize );

		glDrawElements( drawTopologyProperties.primitiveTopology,
		                static_cast<GLsizei>( pIndicesNum ),
		                drawTopologyProperties.indexBufferDataType,
		                indexDataOffset );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
	}

	void GLCommandList::drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		_stateController.applyPipelineStateChanges();

		const auto & drawTopologyProperties = _stateController.getGLDrawTopologyProperties();

		glDrawArrays( drawTopologyProperties.primitiveTopology,
		              static_cast<GLint>( pVerticesOffset ),
		              static_cast<GLsizei>( pVerticesNum ) );
		ts3OpenGLHandleLastError();
	}

	void GLCommandList::drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
	}

	void GLCommandList::executeRenderPassLoadActions()
	{
		if( _currentRenderPassConfiguration.attachmentActionClearMask != 0 )
		{
			smutil::clearRenderPassFramebuffer(
					_stateController.getCurrentRenderTargetBindingInfo(),
					_currentRenderPassConfiguration );
		}
	}

	void GLCommandList::executeRenderPassStoreActions()
	{
		if( _currentRenderPassConfiguration.attachmentActionResolveMask != 0 )
		{
			smutil::resolveRenderPassFramebuffer(
					_stateController.getCurrentRenderTargetBindingInfo(),
					_currentRenderPassConfiguration );
		}
	}

	void GLCommandList::updateShaderInputInlineConstantData( const ShaderInputParameterConstant & pConstantInfo, const void * pConstantData )
	{
		auto constantBaseType = cxdefs::getVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = cxdefs::getVertexAttribFormatLength( pConstantInfo.iFormat );

		const auto * shaderLinkageState = _stateController.getCurrentSeparableStates().shaderLinkageState;
		const auto * glcShaderLinkageState = shaderLinkageState->queryInterface<GLGraphicsShaderLinkageImmutableState>();

		if( glcShaderLinkageState->mGLShaderPipelineObject )
		{
			smutil::updateUniformDataCurrent(
					*( glcShaderLinkageState->mGLShaderPipelineObject ),
					pConstantInfo.iStageIndex,
					constantBaseType,
					constantLength,
					pConstantData );
		}
		else
		{
			smutil::updateUniformDataExplicit(
					*( glcShaderLinkageState->mGLShaderProgramObject ),
					pConstantInfo.iStageIndex,
					constantBaseType,
					constantLength,
					pConstantData );
		}
	}

} // namespace ts3::gpuapi
