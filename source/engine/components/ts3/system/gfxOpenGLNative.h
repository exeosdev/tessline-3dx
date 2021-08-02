
#ifndef __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
#define __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__

#include "openGL.h"

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidOpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32OpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11OpenGL.h"
#endif

namespace ts3
{
namespace system
{

    struct GfxGLDriver
    {
        GfxGLDriverNativeData nativeData;
    };

    struct GfxGLSurface
    {
        GfxGLSurfaceNativeData nativeData;

        GfxGLDriver * coreDevice;
    };

    struct GfxGLRenderContext
    {
        GfxGLRenderContextNativeData nativeData;

        GfxGLDriver * coreDevice;
    };


    TS3_PCL_ATTR_NO_DISCARD GfxGLDriverHandle sysCreateGLDriver();

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
