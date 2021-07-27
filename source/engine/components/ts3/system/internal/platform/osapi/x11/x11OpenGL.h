
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__

#include "x11Window.h"
#include <GL/glx.h>
#include <GL/glxext.h>

namespace ts3
{

	struct SysX11GLSurfaceNativeData : public SysX11WindowNativeData
	{
		GLXFBConfig fbConfig = nullptr;
		XVisualInfo * visualInfo = nullptr;
	};

	struct SysX11GLRenderContextNativeData
	{
		Display * display = nullptr;
		GLXDrawable targetSurface = XID_None;
		GLXContext contextHandle = nullptr;
	};

	struct SysX11GLCoreDeviceNativeData
	{
		struct InitState
		{
            SysX11GLSurfaceNativeData surfaceData;
            SysX11GLRenderContextNativeData contextData;
		};

		InitState * initState = nullptr;
	};

    using SysGLCoreDeviceNativeData = SysX11GLCoreDeviceNativeData;
    using SysGLSurfaceNativeData = SysX11GLSurfaceNativeData;
    using SysGLRenderContextNativeData = SysX11GLRenderContextNativeData;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
