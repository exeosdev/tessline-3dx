
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__

#include "androidEventCore.h"
#include "../../shared/egl/eglGfxOpenGL.h"
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>

namespace ts3
{

	struct SysGLSurfaceNativeData : public EGLSurfaceNativeData, public SysEventSourceNativeData
	{
	};

	struct SysGLContextNativeData : public EGLContextNativeData
	{
	};

	struct SysGLSubsystemNativeData : public EGLSubsystemNativeData
	{
	};

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
