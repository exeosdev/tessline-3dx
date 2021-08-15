
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

    /// @brief
    class GLSystemDriverNativeProxy
    {
    public:
        GLSystemDriverNativeData mNativeData;

    public:
        GLSystemDriverNativeProxy( GLSystemDriver * pParentObject );
        virtual ~GLSystemDriverNativeProxy();

        void initializePlatform();
        void releaseInitState( GLRenderContext & pGLRenderContext );
        void createDisplaySurface( GLDisplaySurface & pSurfaceObject, const GLSurfaceCreateInfo & pCreateInfo );
        void createDisplaySurfaceForCurrentThread( GLDisplaySurface & pSurfaceObject );
        void createRenderContext( GLRenderContext & pRenderContext, GLDisplaySurface & pSurface,
                                  const GLRenderContextCreateInfo & pCreateInfo );
        void createRenderContextForCurrentThread( GLRenderContext & pRenderContext, GLDisplaySurface & pSurface );
    };

    /// @brief
    class GLDisplaySurfaceNativeProxy
    {
    public:
        GLDisplaySurfaceNativeData mNativeData;

    public:
        GLDisplaySurfaceNativeProxy();
        virtual ~GLDisplaySurfaceNativeProxy();

        void swapBuffers();
        WindowSize queryRenderAreaSize() const;
        bool querySurfaceBindStatus() const;
    };

    /// @brief
    class GLRenderContextNativeProxy
    {
    public:
        GLRenderContextNativeData mNativeData;

    public:
        GLRenderContextNativeProxy();
        virtual ~GLRenderContextNativeProxy();

        void bindForCurrentThread();
        bool validateCurrentBinding() const;
    };

} // namespace system
} // namespace ts3

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
