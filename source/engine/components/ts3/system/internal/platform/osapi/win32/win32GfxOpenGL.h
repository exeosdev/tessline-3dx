
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__

#include "win32WindowSystem.h"
#include <GL/wglew.h>

namespace ts3
{

	inline constexpr int cvSysWin32InvalidPixelFormatIndex = -1;
	inline constexpr size_t cvSysWin32MaxWGLPixelFormatsNum = 256u;
	inline constexpr size_t cvSysWin32MaxWGLPixelFormatAttributesNum = 64u;

	struct SysGLSurfaceNativeData : public SysWindowNativeData
	{
		HDC surfaceHandle;
		int pixelFormatIndex;
	};

	struct SysGLRenderContextNativeData
	{
		HGLRC contextHandle;
	};

	struct SysGLDriverNativeData
	{
		struct InitState
		{
			SysGLSurfaceNativeData surfaceData;
			SysGLRenderContextNativeData contextData;
		};

		InitState initState;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
