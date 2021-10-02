
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
#include "state/GL_renderTargetStateObject.h"
#include "state/GL_vertexStreamStateObject.h"
#include <ts3/gpuapi/resources/gpuBuffer.h>
#include <ts3/gpuapi/resources/shader.h>
#include <ts3/gpuapi/resources/texture.h>

namespace ts3::gpuapi
{

	GLCommandList::GLCommandList( GLCommandSystem & pGLCommandSystem, ECommandListType pListType, system::GLRenderContextHandle pSysGLRenderContext )
	: CommandList( pGLCommandSystem, pListType )
	, mSysGLRenderContext( pSysGLRenderContext )
	, _stateController( mGPUDevice.queryInterface<GLGPUDevice>()->getDescriptorCache() )
	{}

	GLCommandList::~GLCommandList() = default;

	void GLCommandList::beginCommandSequence()
	{
		CommandList::beginCommandSequence();
	}

	void GLCommandList::endCommandSequence()
	{
		_stateController.resetInternalState();

		CommandList::endCommandSequence();
	}

	void GLCommandList::executeDeferredContext( CommandContextDeferred & pDeferredContext )
	{
		ts3DebugInterrupt();
	}

	void GLCommandList::clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask )
	{
		if( pAttachmentMask == 0 )
		{
			return;
		}

		const auto & renderTargetClearConfig = getRenderTargetClearConfig();


		if( pAttachmentMask.isSet( E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT ) )
		{
			const GLfloat depthClearValue = renderTargetClearConfig.depthClearValue;
			glClearBufferfv( GL_DEPTH, 0, &depthClearValue );
			ts3GLHandleLastError();
			pAttachmentMask.unset( E_RENDER_TARGET_ATTACHMENT_FLAG_DEPTH_BIT );
		}

		if( pAttachmentMask.isSet( E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT ) )
		{
			const GLint stencilClearValue = renderTargetClearConfig.stencilClearValue;
			glClearBufferiv( GL_STENCIL, 0, &stencilClearValue );
			ts3GLHandleLastError();
			pAttachmentMask.unset( E_RENDER_TARGET_ATTACHMENT_FLAG_STENCIL_BIT );
		}

		for( uint32 colorBufferIndex = 0; ( colorBufferIndex < GPU_SYSTEM_METRIC_RT_MAX_COLOR_ATTACHMENTS_NUM ) && ( pAttachmentMask != 0 ); ++colorBufferIndex )
		{
			auto colorBufferMask = E_RENDER_TARGET_ATTACHMENT_FLAG_COLOR_0_BIT << colorBufferIndex;
			if( pAttachmentMask.isSet( colorBufferMask ) )
			{
				glClearBufferfv( GL_COLOR, colorBufferIndex, &( renderTargetClearConfig.colorClearValue.rgbaArray[0] ) );
				ts3GLHandleLastError();
				pAttachmentMask.unset( colorBufferMask );
			}
		}

		ts3DebugAssert( pAttachmentMask == 0 );
	}

	void GLCommandList::setViewport( const ViewportDesc & pViewportDesc )
	{
		glViewport( pViewportDesc.vpRect.opX, pViewportDesc.vpRect.opY, pViewportDesc.vpRect.sizeX, pViewportDesc.vpRect.sizeY );
		ts3GLHandleLastError();

		glDepthRangef( pViewportDesc.depthRange.zNear, pViewportDesc.depthRange.zFar );
		ts3GLHandleLastError();
	}

	bool GLCommandList::setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO )
	{
		return _stateController.setGraphicsPipelineStateObject( pGraphicsPipelineSO );
	}

	bool GLCommandList::setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO )
	{
		return _stateController.setVertexStreamStateObject( pVertexStreamSO );
	}

	bool GLCommandList::setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		const auto & renderTargetLayout = commonConfig.soGraphicsPipeline->mRenderTargetLayout;

		auto * openglRenderTargetSO = pRenderTargetSO.queryInterface<GLRenderTargetStateObject>();
		if( !checkRenderTargetLayoutCompatibility( openglRenderTargetSO->mRTResourceBinding, renderTargetLayout ) )
		{
			ts3DebugInterrupt();
			return false;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, openglRenderTargetSO->mGLFramebufferObject->mGLHandle );
		ts3GLHandleLastError();

		return true;
	}

	bool GLCommandList::setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
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
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
			ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::CBVConstantBuffer );

			if( descriptorInfo.dShaderVisibilityMask != 0 )
			{
				auto * openglBuffer = pConstantBuffer.queryInterface<GLGPUBuffer>();

				glBindBufferBase( GL_UNIFORM_BUFFER, descriptorInfo.uResourceInfo.resourceBaseRegisterIndex, openglBuffer->mGLBufferObject->mGLHandle );
				ts3GLHandleLastError();

				return true;
			}
		}
		return false;
	}

	bool GLCommandList::setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Resource );
			ts3DebugAssert( descriptorInfo.uResourceInfo.resourceType == EShaderInputResourceType::SRVTextureImage );

			auto * openglTexture = pTexture.queryInterface<GLTexture>();

			glActiveTexture( GL_TEXTURE0 + descriptorInfo.uResourceInfo.resourceBaseRegisterIndex );
			ts3GLHandleLastError();

			glBindTexture( openglTexture->mGLTextureObject->mGLTextureBindTarget, openglTexture->mGLTextureObject->mGLHandle );
			ts3GLHandleLastError();

			return true;
		}
		return false;
	}

	bool GLCommandList::setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler )
	{
		const auto & commonConfig = _stateController.getCommonConfig();
		if( const auto & inputSignature = commonConfig.soGraphicsPipeline->mShaderInputSignature )
		{
			const auto & descriptorInfo = inputSignature.getDescriptorInfo( pParamRefID );
			ts3DebugAssert( descriptorInfo.dDescriptorType == EShaderInputDescriptorType::Sampler );

			if( descriptorInfo.dShaderVisibilityMask != 0 )
			{
				auto * openglSampler = pSampler.queryInterface<GLSampler>();

				glBindSampler( descriptorInfo.uSamplerInfo.samplerBindingIndex, openglSampler->mGLSamplerObject->mGLHandle );
				ts3GLHandleLastError();

				return true;
			}
		}
		return false;
	}

	void GLCommandList::drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset )
	{
		updatePipelineState();

		const auto & openglPipelineConfig = _stateController.getGLPipelineConfig();
		const auto * indexDataOffset = reinterpret_cast<void*>( pIndicesOffset * openglPipelineConfig.indexBufferElementByteSize );

		glDrawElements( openglPipelineConfig.primitiveTopology,
		                static_cast<GLsizei>( pIndicesNum ),
		                openglPipelineConfig.indexBufferDataType,
		                indexDataOffset );
		ts3GLHandleLastError();
	}

	void GLCommandList::drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset )
	{
	}

	void GLCommandList::drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset )
	{
		updatePipelineState();

		const auto & openglPipelineConfig = _stateController.getGLPipelineConfig();

		glDrawArrays( openglPipelineConfig.primitiveTopology,
		              static_cast<GLint>( pVerticesOffset ),
		              static_cast<GLsizei>( pVerticesNum ) );
		ts3GLHandleLastError();
	}

	void GLCommandList::drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset )
	{
	}

	void GLCommandList::updatePipelineState()
	{
		_stateController.updatePipelineState();
	}

	void GLCommandList::updateShaderInputInlineConstantData( const ShaderInputParameterConstant & pConstantInfo, const void * pData )
	{
		auto constantBaseType = ecGetVertexAttribFormatBaseDataType( pConstantInfo.iFormat );
		auto constantLength = ecGetVertexAttribFormatLength( pConstantInfo.iFormat );

		const auto & commonConfig = _stateController.getCommonConfig();
		auto * openglGraphicsPSO = commonConfig.soGraphicsPipeline->queryInterface<GLGraphicsPipelineStateObject>();
		for( const auto & shaderInfo : openglGraphicsPSO->mShaderBinding.activeStageList )
		{
			auto stageFlag = getGraphicsShaderStageFlag( static_cast<EShaderType>( shaderInfo.stageID ) );
			if( pConstantInfo.iVisibilityMask.isSet( stageFlag ) )
			{
				auto * openglShader = shaderInfo.shaderObject->queryInterface<GLShader>();
				updateUniformDataExplicit( *( openglShader->mGLShaderProgramObject ), pConstantInfo.iStageIndex, constantBaseType, constantLength, pData );
			}
		}
	}

	void GLCommandList::updateUniformDataCurrent( uint32 pUniformIndex, EBaseDataType pBaseType, uint32 pLength, const void * pData )
	{
		if( ( pLength == 0 ) || !pData )
		{
			return;
		}

		if( pBaseType == EBaseDataType::Float32 )
		{
			const auto * floatData = reinterpret_cast<const float *>( pData );
			switch( pLength )
			{
				case 1: glUniform1fv( pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 2: glUniform2fv( pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 3: glUniform3fv( pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 4: glUniform4fv( pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
			}
		}
		else if( pBaseType == EBaseDataType::Int32 )
		{
			const auto * intData = reinterpret_cast<const GLint *>( pData );
			switch( pLength )
			{
				case 1: glUniform1iv( pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 2: glUniform2iv( pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 3: glUniform3iv( pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 4: glUniform4iv( pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
			}
		}
		else if( pBaseType == EBaseDataType::Uint32 )
		{
			const auto * uintData = reinterpret_cast<const GLuint *>( pData );
			switch( pLength )
			{
				case 1: glUniform1uiv( pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 2: glUniform2uiv( pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 3: glUniform3uiv( pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 4: glUniform4uiv( pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
			}
		}
	}

	void GLCommandList::updateUniformDataExplicit( GLShaderProgramObject & pProgram, uint32 pUniformIndex, EBaseDataType pBaseType, uint32 pLength, const void * pData )
	{
		if( ( pLength == 0 ) || !pData )
		{
			return;
		}

		if( pBaseType == EBaseDataType::Float32 )
		{
			const auto * floatData = reinterpret_cast<const float *>( pData );
			switch( pLength )
			{
				case 1: glProgramUniform1fv( pProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 2: glProgramUniform2fv( pProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 3: glProgramUniform3fv( pProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
				case 4: glProgramUniform4fv( pProgram.mGLHandle, pUniformIndex, 1, floatData ); ts3GLHandleLastError(); break;
			}
		}
		else if( pBaseType == EBaseDataType::Int32 )
		{
			const auto * intData = reinterpret_cast<const GLint *>( pData );
			switch( pLength )
			{
				case 1: glProgramUniform1iv( pProgram.mGLHandle, pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 2: glProgramUniform2iv( pProgram.mGLHandle, pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 3: glProgramUniform3iv( pProgram.mGLHandle, pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
				case 4: glProgramUniform4iv( pProgram.mGLHandle, pUniformIndex, 1, intData ); ts3GLHandleLastError(); break;
			}
		}
		else if( pBaseType == EBaseDataType::Uint32 )
		{
			const auto * uintData = reinterpret_cast<const GLuint *>( pData );
			switch( pLength )
			{
				case 1: glProgramUniform1uiv( pProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 2: glProgramUniform2uiv( pProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 3: glProgramUniform3uiv( pProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
				case 4: glProgramUniform4uiv( pProgram.mGLHandle, pUniformIndex, 1, uintData ); ts3GLHandleLastError(); break;
			}
		}
	}

}
