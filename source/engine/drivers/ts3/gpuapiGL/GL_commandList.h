
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_CONTEXT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_CONTEXT_H__

#include "GL_prerequisites.h"
#include "state/GL_pipelineStateDesc.h"
#include "state/GL_pipelineStateController.h"
#include <ts3/gpuapi/commandList.h>

namespace ts3::gpuapi
{

	/// @brief
	class TS3_GPUAPI_CLASS GLCommandList : public CommandList
	{
	public:
		SysGLContextHandle const mSysGLContext;

	public:
		GLCommandList( GLCommandSystem & pGLCommandSystem, ECommandListType pListType, SysGLContextHandle pSysGLContext );
		virtual ~GLCommandList();

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) override;

		virtual void clearRenderTarget( Bitmask<ERenderTargetAttachmentFlags> pAttachmentMask ) override;
		virtual void setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setGraphicsPipelineStateObject( const GraphicsPipelineStateObject & pGraphicsPipelineSO ) override;
		virtual bool setVertexStreamStateObject( const VertexStreamStateObject & pVertexStreamSO ) override;
		virtual bool setRenderTargetStateObject( const RenderTargetStateObject & pRenderTargetSO ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;
		virtual void drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) override;
		virtual void drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) override;

	protected:
		void updatePipelineState();
		void updateShaderInputInlineConstantData( const ShaderInputParameterConstant & pConstantInfo, const void * pData );

		static void updateUniformDataCurrent( uint32 pUniformIndex, EBaseDataType pBaseType, uint32 pLength, const void * pData );
		static void updateUniformDataExplicit( GLShaderProgramObject & pProgram, uint32 pUniformIndex, EBaseDataType pBaseType, uint32 pLength, const void * pData );

	private:
		GLGraphicsPipelineStateController _stateController;
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_CONTEXT_H__
