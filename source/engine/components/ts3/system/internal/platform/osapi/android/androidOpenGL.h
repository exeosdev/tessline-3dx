
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__

#include "androidEventCore.h"
#include "../../shared/egl/eglOpenGL.h"
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>

namespace ts3
{

	struct GLSurfaceNativeData : public EGLSurfaceNativeData, public EventSourceNativeData
	{
	};

	struct GLRenderContextNativeData : public EGLRenderContextNativeData
	{
	};

	struct GLDriverNativeData : public EGLDriverNativeData
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
