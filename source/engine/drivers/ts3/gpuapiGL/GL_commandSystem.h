
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_MANAGER_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_MANAGER_H__

#include "GL_prerequisites.h"
#include <ts3/gpuapi/commandSystem.h>

namespace ts3::gpuapi
{

	struct GLCommandSyncData
	{
		GLsync openglSyncFence = nullptr;
	};

	TS3GX_GL_API void releaseGLCommandSyncData( void * pSyncData );

	/// @brief
	class TS3GX_GL_CLASS GLCommandSystem : public CommandSystem
	{
	public:
		explicit GLCommandSystem( GLGPUDevice & pGLGPUDevice );
		virtual ~GLCommandSystem();

		virtual std::unique_ptr<CommandContext> acquireCommandContext( ECommandContextType pContextType ) override;

		virtual CommandSync submitContext( CommandContextDirect & pContext, const CommandContextSubmitInfo & pSubmitInfo ) override;

		// OpenGL-specific thing coming from the fact, that context creation requires target surface (Drawable on X11,
		// HDC on Win32, EGLSurface on Android, etc.). GPUDevice::setPresentationLayer was actually introduced only to
		// make tis possible in at least a bit natural way. So, when PresentationLayer is set, GL-level onSet() gets
		// the surface and passes it here, so CmdManager has the surface when a CmdContext is created.
		// Also, that is the reason why no context is created in initialize() function and the requirement of having
		// a surface bound to the device when acquireContext() is called.
		void setTargetGLSurface( system::OpenGLDisplaySurfaceHandle pSysGLDisplaySurface );

	private:
		GLCommandList * acquireCommandList( ECommandExecutionMode pCommandExecutionMode );
		bool initializeMainCommandList();

		static system::OpenGLRenderContextHandle createSysGLRenderContext( GLGPUDevice & pGLGPUDevice, system::OpenGLDisplaySurfaceHandle pSysGLDisplaySurface );

	protected:
		system::OpenGLDisplaySurfaceHandle _targetSysGLSurface;
		GLCommandListHandle _mainCommandList;
	};
	
} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_GPU_CMD_MANAGER_H__
