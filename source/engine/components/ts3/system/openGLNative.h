
#ifndef __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
#define __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__

#include "openGLDriver.h"
#include <list>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
#  include "internal/platform/osapi/android/androidOpenGL.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_WIN32 )
#  include "internal/platform/osapi/win32/win32OpenGLDriver.h"
#elif( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_X11 )
#  include "internal/platform/osapi/x11/x11OpenGLDriver.h"
#endif

namespace ts3::system
{

    struct OpenGLSystemDriver::ObjectInternalData
    {
        OpenGLSystemDriver * parentDriver = nullptr;
        OpenGLSystemDriverNativeData nativeDataPriv;
        Version supportedRuntimeVersion;
        std::vector<OpenGLDisplaySurface *> internalDisplaySurfaceList;
        std::vector<OpenGLRenderContext *> internalRenderContextList;

        explicit ObjectInternalData( OpenGLSystemDriver * pSystemDriver )
        : parentDriver( pSystemDriver )
        , supportedRuntimeVersion{ 0, 0 }
        {}
    };

    struct OpenGLDisplaySurface::ObjectInternalData
    {
        OpenGLDisplaySurface * parentSurface = nullptr;
        GLDisplaySurfaceNativeData nativeDataPriv;
        bool internalOwnershipFlag = true;
        bool internalInvalidateFlag = false;

        explicit ObjectInternalData( OpenGLDisplaySurface * pDisplaySurface )
        : parentSurface( pDisplaySurface )
        {}
    };

    struct OpenGLRenderContext::ObjectInternalData
    {
        OpenGLRenderContext * parentContext = nullptr;
        GLRenderContextNativeData nativeDataPriv;
        bool internalOwnershipFlag = true;
        bool internalInvalidateFlag = false;

        explicit ObjectInternalData( OpenGLRenderContext * pRenderContext )
        : parentContext( pRenderContext )
        {}
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
