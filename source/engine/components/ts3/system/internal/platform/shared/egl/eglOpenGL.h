
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace ts3::system
{

    constexpr EGLConfig cvEGLInvalidConfigID = nullptr;
    constexpr size_t cvEGLMaxEGLConfigsNum = 256u;
    constexpr size_t cvEGLMaxEGLConfigAttributesNum = 64u;

    struct EGLDisplaySurfaceNativeData
    {
        EGLDisplay eDisplay = EGL_NO_DISPLAY;
        EGLNativeWindowType eNativeWindow = nullptr;
        EGLConfig eFBConfig = nullptr;
        EGLSurface eSurfaceHandle = nullptr;
    };

    struct EGLRenderContextNativeData
    {
        EGLDisplay eDisplay = EGL_NO_DISPLAY;
        EGLContext eContextHandle = nullptr;
    };

    struct EGLDriverNativeData
    {
        EGLDisplay eDisplay = EGL_NO_DISPLAY;
        EGLNativeWindowType eNativeWindow = nullptr;
        Version eglVersion;
    };

    void nativeEGLInitializeGLDriver( EGLDriverNativeData & pEGLDriverNativeData );

    void nativeEGLReleaseGLDriver( EGLDriverNativeData & pEGLDriverNativeData );

    EGLConfig nativeEGLChooseCoreFBConfig( EGLDisplay pDisplay, const VisualConfig & pVisualConfig );

    EGLint nativeEGLQueryFBConfigAttribute( EGLDisplay pEGLDisplay, EGLConfig pEGLConfig, EGLenum pAttribute );

    void nativeEGLCreateSurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
                                 EGLDisplay pEGLDisplay,
                                 EGLNativeWindowType pWindow,
                                 EGLConfig pEGLConfig,
                                 const VisualConfig & pVisualConfig );

    void nativeEGLCreateSurfaceForCurrentThread( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData );

    void nativeEGLDestroySurface( EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData );

    void nativeEGLCreateCoreContext( EGLRenderContextNativeData & pEGLContextNativeData,
                                     const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData,
                                     const GLRenderContextCreateInfo & pCreateInfo );

    void nativeEGLCreateCoreContextForCurrentThread( EGLRenderContextNativeData & pEGLContextNativeData );

    void nativeEGLDestroyRenderContext( EGLRenderContextNativeData & pEGLContextNativeData );

    void nativeEGLBindContextForCurrentThread( const EGLRenderContextNativeData & pEGLContextNativeData,
                                               const EGLDisplaySurfaceNativeData & pEGLSurfaceNativeData );

} // namespace ts3::system

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__
