
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__

#include "androidEventCore.h"
#include "../../shared/egl/eglGfxOpenGL.h"
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>

namespace ts3
{

	struct GfxGLSurfaceNativeData : public EGLSurfaceNativeData, public EventSourceNativeData
	{
	};

	struct GfxGLRenderContextNativeData : public EGLRenderContextNativeData
	{
	};

	struct GfxGLDriverNativeData : public EGLDriverNativeData
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
