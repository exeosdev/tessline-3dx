
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__

#include "win32Windowtem.h"
#include <GL/wglew.h>

namespace ts3
{

	inline constexpr int cvWin32InvalidPixelFormatIndex = -1;
	inline constexpr size_t cvWin32MaxWGLPixelFormatsNum = 256u;
	inline constexpr size_t cvWin32MaxWGLPixelFormatAttributesNum = 64u;

	struct GfxGLSurfaceNativeData : public WindowNativeData
	{
		HDC surfaceHandle;
		int pixelFormatIndex;
	};

	struct GfxGLRenderContextNativeData
	{
		HGLRC contextHandle;
	};

	struct GfxGLDriverNativeData
	{
		struct InitState
		{
			GfxGLSurfaceNativeData surfaceData;
			GfxGLRenderContextNativeData contextData;
		};

		InitState initState;
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
