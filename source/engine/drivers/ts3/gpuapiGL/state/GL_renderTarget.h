
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/gpuapi/state/renderTargetImmutableStates.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLFramebufferObject );
	ts3GLDeclareOpenGLObjectHandle( GLRenderbufferObject );

	struct GLRenderTargetBindingFBOData
	{
		GLFramebufferObjectHandle renderFBO;
		GLFramebufferObjectHandle resolveFBO;
	};

	struct GLRenderTargetBindingDefinition
	{
		GLRenderTargetBindingFBOData fboData;
		RenderTargetLayout rtLayout;
	};

	struct GLRenderTargetBindingInfo
	{
		const GLFramebufferObject * renderFBO = nullptr;
		const GLFramebufferObject * resolveFBO = nullptr;
		const RenderTargetLayout * rtLayout = nullptr;
	};

	class GLRenderTargetBindingImmutableState : public RenderTargetBindingImmutableState
	{
	public:
		GLRenderTargetBindingFBOData const mGLFBOData;

	public:
		GLRenderTargetBindingImmutableState(
				GLGPUDevice & pGPUDevice,
				const RenderTargetLayout & pRenderTargetLayout,
				GLRenderTargetBindingFBOData pGLFBOData );

		virtual ~GLRenderTargetBindingImmutableState();

		TS3_ATTR_NO_DISCARD GLRenderTargetBindingInfo getGLRenderTargetBindingInfo() const;

		static GpaHandle<GLRenderTargetBindingImmutableState> createInstance(
			GLGPUDevice & pGPUDevice,
			const RenderTargetBindingDefinition & pBindingDefinition );

		static GpaHandle<GLRenderTargetBindingImmutableState> createForScreen(
			GLGPUDevice & pGPUDevice,
			const RenderTargetLayout & pRenderTargetLayout );
	};


	namespace smutil
	{

		TS3_ATTR_NO_DISCARD GLRenderTargetBindingInfo getGLRenderTargetBindingInfo(
				const GLRenderTargetBindingDefinition & pBindingDefinition );

		TS3_ATTR_NO_DISCARD GLRenderTargetBindingDefinition translateRenderTargetBindingDefinition(
				const RenderTargetBindingDefinition & pBindingDefinition );

		TS3_ATTR_NO_DISCARD GLFramebufferObjectHandle createFramebufferObject(
				const RenderTargetBindingDefinition & pBindingDefinition,
				Bitmask<ERTAttachmentFlags> pAttachmentMask );

		TS3_ATTR_NO_DISCARD RenderTargetLayout translateSystemVisualConfigToRenderTargetLayout( const system::VisualConfig & pSysVisualConfig );

		void clearRenderPassFramebuffer( const RenderPassConfiguration & pRenderPassConfiguration );

		void resolveRenderPassFramebuffer(
				const GLRenderTargetBindingInfo & pRTBindingInfo,
				const RenderPassConfiguration & pRenderPassConfiguration );

	}

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_RENDER_TARGET_STATE_OBJECT_H__
