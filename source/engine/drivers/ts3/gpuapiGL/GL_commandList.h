
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
	class GLCommandList : public CommandList
	{
	public:
		system::OpenGLRenderContextHandle const mSysGLRenderContext;

	public:
		GLCommandList(
			GLCommandSystem & pGLCommandSystem,
			ECommandListType pListType,
			system::OpenGLRenderContextHandle pSysGLRenderContext,
			GLGraphicsPipelineStateController & pStateController );

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

		virtual void cmdDrawDirectIndexed( native_uint pIndicesNum, native_uint pIndicesOffset ) override;
		virtual void cmdDrawDirectIndexedInstanced( native_uint pIndicesNumPerInstance, native_uint pInstancesNum, native_uint pIndicesOffset ) override;
		virtual void cmdDrawDirectNonIndexed( native_uint pVerticesNum, native_uint pVerticesOffset ) override;
		virtual void cmdDrawDirectNonIndexedInstanced( native_uint pVerticesNumPerInstance, native_uint pInstancesNum, native_uint pVerticesOffset ) override;

		virtual void cmdExecuteDeferredContext( CommandContextDeferred & pDeferredContext ) override;

	protected:
		void executeRenderPassLoadActions();
		void executeRenderPassStoreActions();

	private:
		GLGraphicsPipelineStateController * _graphicsPipelineStateControllerGL;
		RenderPassConfiguration _currentRenderPassConfiguration;
	};

	class GLCommandListCore : public GLCommandList
	{
	public:
		GLCommandListCore(
				GLCommandSystem & pGLCommandSystem,
				ECommandListType pListType,
				system::OpenGLRenderContextHandle pSysGLRenderContext )
		: GLCommandList( pGLCommandSystem, pListType, pSysGLRenderContext, _graphicsPipelineStateControllerCore )
		{}

	private:
		GLGraphicsPipelineStateControllerCore _graphicsPipelineStateControllerCore;
	};

	class GLCommandListCompat : public GLCommandList
	{
	public:
		GLCommandListCompat(
				GLCommandSystem & pGLCommandSystem,
				ECommandListType pListType,
				system::OpenGLRenderContextHandle pSysGLRenderContext )
		: GLCommandList( pGLCommandSystem, pListType, pSysGLRenderContext, _graphicsPipelineStateControllerCompat )
		{}

	private:
		GLGraphicsPipelineStateControllerCompat _graphicsPipelineStateControllerCompat;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_COMMAND_LIST_H__
