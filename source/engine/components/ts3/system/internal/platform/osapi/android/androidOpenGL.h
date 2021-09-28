
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__

#include "androidEventSystem.h"
#include <ts3/system/internal/platform/shared/egl/eglOpenGL.h>
#include <GLES3/gl31.h>

namespace ts3::system
{

	struct GLDisplaySurfaceNativeData : public EGLDisplaySurfaceNativeData, public EventSourceNativeData
	{
	};

	struct GLRenderContextNativeData : public EGLRenderContextNativeData, public AndroidNativeDataCommon
	{
	};

	struct GLSystemDriverNativeData : public EGLDriverNativeData, public AndroidNativeDataCommon
	{
	};

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_ANDROID_OPENGL_H__
