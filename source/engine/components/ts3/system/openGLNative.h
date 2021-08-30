
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

namespace ts3::system
{

    /// @brief
    class GLSystemDriverNativeImpl : public GLSystemDriver
    {
    public:
        GLSystemDriverNativeData mNativeData;

    public:
        explicit GLSystemDriverNativeImpl( DisplayManagerHandle pDisplayManager );
        virtual ~GLSystemDriverNativeImpl();

        virtual void _nativeInitializePlatform() override;
        virtual void _nativeReleaseInitState( GLRenderContext & pRenderContext ) override;
        virtual void _nativeCreateDisplaySurface( GLDisplaySurface & pDisplaySurface, const GLDisplaySurfaceCreateInfo & pCreateInfo ) override;
        virtual void _nativeCreateDisplaySurfaceForCurrentThread( GLDisplaySurface & pDisplaySurface ) override;
        virtual void _nativeCreateRenderContext( GLRenderContext & pRenderContext, const GLDisplaySurface & pSurface, const GLRenderContextCreateInfo & pCreateInfo ) override;
        virtual void _nativeCreateRenderContextForCurrentThread( GLRenderContext & pRenderContext ) override;
        virtual bool _nativeIsRenderContextBound() const override;
        virtual bool _nativeIsRenderContextBound( const GLRenderContext & pRenderContext ) const override;
    };

    /// @brief
    class GLDisplaySurfaceNativeImpl : public GLDisplaySurface
    {
    public:
        GLDisplaySurfaceNativeData mNativeData;

    public:
        explicit GLDisplaySurfaceNativeImpl( GLSystemDriverHandle pDriver );
        virtual ~GLDisplaySurfaceNativeImpl();

        virtual void _nativeSwapBuffers() override final;
        virtual void _nativeDestroy() override final;
        virtual void _nativeQueryRenderAreaSize( WindowSize & pOutSize ) const override final;
        virtual bool _nativeIsValid() const override final;
    };

    /// @brief
    class GLRenderContextNativeImpl : public GLRenderContext
    {
    public:
        GLRenderContextNativeData mNativeData;

    public:
        explicit GLRenderContextNativeImpl( GLSystemDriverHandle pDriver );
        virtual ~GLRenderContextNativeImpl();

        virtual void _nativeBindForCurrentThread( const GLDisplaySurface & pSurface ) override final;
        virtual void _nativeDestroy() override final;
        virtual bool _nativeIsCurrent() const override final;
        virtual bool _nativeIsValid() const override final;
    };

} // namespace ts3::system

#endif // __TS3_SYSTEM_GFX_OPENGL_NATIVE_H__
