
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__

#include "x11Window.h"
#include <GL/glx.h>
#include <GL/glxext.h>

namespace ts3
{
namespace system
{

	struct GLSurfaceNativeData : public WindowNativeData
	{
		GLXFBConfig fbConfig = nullptr;
		XVisualInfo * visualInfo = nullptr;
	};

	struct GLRenderContextNativeData
	{
		Display * display = nullptr;
		GLXDrawable targetSurface = XID_None;
		GLXContext contextHandle = nullptr;
	};

	struct GLDriverNativeData
	{
		struct InitState
		{
            X11GLSurfaceNativeData surfaceData;
            X11GLRenderContextNativeData contextData;
		};

		InitState * initState = nullptr;
	};

    using X11GLDriverNativeData = GLDriverNativeData;
    using X11GLSurfaceNativeData = GLSurfaceNativeData;
    using X11GLRenderContextNativeData = GLRenderContextNativeData;

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
