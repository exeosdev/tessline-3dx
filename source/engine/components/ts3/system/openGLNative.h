
#ifndef __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
#define __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__

#include "openGL.h"
#include <list>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidOpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32OpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11OpenGL.h"
#endif

namespace ts3::system
{

    struct GLSystemDriver::ObjectInternalData
    {
        GLSystemDriver * parentDriver = nullptr;
        GLSystemDriverNativeData nativeDataPriv;
        Version supportedRuntimeVersion;

        explicit ObjectInternalData( GLSystemDriver * pSystemDriver )
        : parentDriver( pSystemDriver )
        , supportedRuntimeVersion{ 0, 0 }
        {}
    };

    struct GLDisplaySurface::ObjectInternalData
    {
        GLDisplaySurface * parentSurface = nullptr;
        GLDisplaySurfaceNativeData nativeDataPriv;
        bool internalOwnershipFlag = true;

        explicit ObjectInternalData( GLDisplaySurface * pDisplaySurface )
        : parentSurface( pDisplaySurface )
        {}
    };

    struct GLRenderContext::ObjectInternalData
    {
        GLRenderContext * parentContext = nullptr;
        GLRenderContextNativeData nativeDataPriv;
        bool internalOwnershipFlag = true;

        explicit ObjectInternalData( GLRenderContext * pRenderContext )
        : parentContext( pRenderContext )
        {}
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
