
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__

#include "win32Window.h"
#include <GL/wglew.h>

namespace ts3
{
namespace system
{

	inline constexpr int cvWin32InvalidPixelFormatIndex = -1;
	inline constexpr size_t cvWin32MaxWGLPixelFormatsNum = 256u;
	inline constexpr size_t cvWin32MaxWGLPixelFormatAttributesNum = 64u;

	struct GLDisplaySurfaceNativeData : public WindowNativeData
	{
		HDC hdc;
		int pixelFormatIndex;
	};

	struct GLRenderContextNativeData
	{
		HGLRC contextHandle;
	};

	struct GLSystemDriverNativeData
	{
		struct InitState
		{
		    GLDisplaySurfaceNativeData surfaceData;
			GLRenderContextNativeData contextData;
		};

		InitState initState;
	};

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_WIN32_GFX_OPENGL_H__
