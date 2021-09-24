
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_H__

#include "x11Window.h"
#include <GL/glx.h>
#include <GL/glxext.h>

namespace ts3::system
{

	struct GLDisplaySurfaceNativeData : public WindowNativeData
	{
		GLXFBConfig fbConfig = nullptr;
		XVisualInfo * visualInfo = nullptr;
	};

	struct GLRenderContextNativeData : public X11NativeDataCommon
	{
		GLXContext contextHandle = nullptr;
	};

	struct GLSystemDriverNativeData : public X11NativeDataCommon
	{
		struct InitState
		{
            GLDisplaySurfaceNativeData surfaceData;
            GLRenderContextNativeData contextData;
		};

		InitState * initState = nullptr;
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_X11_OPENGL_H__
