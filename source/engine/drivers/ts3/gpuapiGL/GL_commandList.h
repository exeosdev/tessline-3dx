
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_COMMAND_LIST_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_COMMAND_LIST_H__

#include "GL_prerequisites.h"
#include "state/GL_pipelineStateController.h"
#include <ts3/gpuapi/commandList.h>

namespace ts3::gpuapi
{

	class GLShaderProgramObject;

	/// @brief
	class TS3_GPUAPI_CLASS GLCommandList : public CommandList
	{
	public:
		system::OpenGLRenderContextHandle const mSysGLRenderContext;

	public:
		GLCommandList( GLCommandSystem & pGLCommandSystem, ECommandListType pListType, system::OpenGLRenderContextHandle pSysGLRenderContext );
		virtual ~GLCommandList();

		virtual bool beginRenderPass( const RenderPassConfigurationImmutableState & pRenderPassState ) override;
		virtual bool beginRenderPass( const RenderPassConfigurationDynamicState & pRenderPassState ) override;
		virtual void endRenderPass() override;

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void executeDeferredContext( CommandContextDeferred & pDeferredContext ) override;

		virtual void setViewport( const ViewportDesc & pViewportDesc ) override;
		virtual bool setShaderConstant( shader_input_ref_id_t pParamRefID, const void * pData ) override;
		virtual bool setShaderConstantBuffer( shader_input_ref_id_t pParamRefID, GPUBuffer & pConstantBuffer ) override;
		virtual bool setShaderTextureImage( shader_input_ref_id_t pParamRefID, Texture & pTexture ) override;
		virtual bool setShaderTextureSampler( shader_input_ref_id_t pParamRefID, Sampler & pSampler ) override;

		virtual void drawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) override;
		virtual void drawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) override;
		virtual void drawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) override;

	protected:
		void executeRenderPassLoadActions();
		void executeRenderPassStoreActions();
		void updateShaderInputInlineConstantData( const ShaderInputParameterConstant & pConstantInfo, const void * pConstantData );

	private:
		GLGraphicsPipelineStateController _stateController;
		RenderPassConfiguration _currentRenderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_COMMAND_LIST_H__
