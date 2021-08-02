
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__

#include "x11Window.h"
#include <GL/glx.h>
#include <GL/glxext.h>

namespace ts3
{

	struct SysGLSurfaceNativeData : public SysWindowNativeData
	{
		GLXFBConfig fbConfig = nullptr;
		XVisualInfo * visualInfo = nullptr;
	};

	struct SysGLRenderContextNativeData
	{
		Display * display = nullptr;
		GLXDrawable targetSurface = XID_None;
		GLXContext contextHandle = nullptr;
	};

	struct SysGLDriverNativeData
	{
		struct InitState
		{
            SysX11GLSurfaceNativeData surfaceData;
            SysX11GLRenderContextNativeData contextData;
		};

		InitState * initState = nullptr;
	};

    using SysX11GLDriverNativeData = SysGLDriverNativeData;
    using SysX11GLSurfaceNativeData = SysGLSurfaceNativeData;
    using SysX11GLRenderContextNativeData = SysGLRenderContextNativeData;

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
