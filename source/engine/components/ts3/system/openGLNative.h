
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

    struct GLSystemDriver::ObjectPrivateData
    {
        using GLDisplaySurfaceList = std::list<GLDisplaySurface>;
        using GLRenderContextList = std::list<GLRenderContext>;
        GLSystemDriverNativeData nativeDataPriv;
        GLDisplaySurfaceList displaySurfaceList;
        GLRenderContextList renderContextList;
    };

    struct GLDisplaySurface::ObjectPrivateData
    {
        using GLDisplaySurfaceRef = GLSystemDriver::ObjectPrivateData::GLDisplaySurfaceList::iterator;
        GLDisplaySurfaceNativeData nativeDataPriv;
        GLDisplaySurfaceRef driverListRef;
        bool internalOwnershipFlag = true;
    };

    struct GLRenderContext::ObjectPrivateData
    {
        using GLRenderContextRef = GLSystemDriver::ObjectPrivateData::GLRenderContextList::iterator;
        GLRenderContextNativeData nativeDataPriv;
        GLRenderContextRef driverListRef;
        bool internalOwnershipFlag = true;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
