
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__

#include "x11WindowSystem.h"
#include <GL/glx.h>
#include <GL/glxext.h>

namespace ts3
{

	constexpr GLXFBConfigID cvSysX11InvalidFBConfigID = -1;
	constexpr size_t cvSysX11MaxGLXFBConfigsNum = 256u;
	constexpr size_t cvSysX11MaxGLXFBConfigAttributesNum = 64u;

	struct SysGLSurfaceNativeData : public SysWindowNativeData
	{
		GLXFBConfig glxFBConfig = nullptr;
		XVisualInfo * xvisualInfo = nullptr;
	};

	struct SysGLContextNativeData
	{
		Display * display;
		GLXDrawable targetSurface;
		GLXContext contextHandle;
	};

	struct SysGLSubsystemNativeData
	{
		struct InitState
		{
			SysGLSurfaceNativeData surfaceData;
			SysGLContextNativeData contextData;
		};

		InitState initState;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_GFX_OPENGL_H__
