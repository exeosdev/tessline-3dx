
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

		virtual bool beginRenderPass(
				const RenderPassConfigurationImmutableState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual bool beginRenderPass(
				const RenderPassConfigurationDynamicState & pRenderPassState,
				Bitmask<ECommandListActionFlags> pFlags ) override;

		virtual void endRenderPass() override;

		virtual void beginCommandSequence() override;
		virtual void endCommandSequence() override;

		virtual void cmdDrawDirectIndexed( uint32 pIndicesNum, uint32 pIndicesOffset ) override;
		virtual void cmdDrawDirectIndexedInstanced( uint32 pIndicesNumPerInstance, uint32 pInstancesNum, uint32 pIndicesOffset ) override;
		virtual void cmdDrawDirectNonIndexed( uint32 pVerticesNum, uint32 pVerticesOffset ) override;
		virtual void cmdDrawDirectNonIndexedInstanced( uint32 pVerticesNumPerInstance, uint32 pInstancesNum, uint32 pVerticesOffset ) override;

		virtual void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext ) override;

	protected:
		void executeRenderPassLoadActions();
		void executeRenderPassStoreActions();

	private:
		GLGraphicsPipelineStateController _stateController;
		RenderPassConfiguration _currentRenderPassConfiguration;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_COMMAND_LIST_H__
