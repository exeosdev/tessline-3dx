
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

    struct SysGLCoreDevice
    {
        SysGLCoreDeviceNativeData nativeData;
    };

    struct SysGLSurface
    {
        SysGLSurfaceNativeData nativeData;

        SysGLCoreDevice * coreDevice;
    };

    struct SysGLRenderContext
    {
        SysGLRenderContextNativeData nativeData;

        SysGLCoreDevice * coreDevice;
    };


    TS3_PCL_ATTR_NO_DISCARD SysGLCoreDeviceHandle sysCreateGLCoreDevice();

}

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
