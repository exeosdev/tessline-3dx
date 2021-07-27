
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace ts3
{

    constexpr EGLConfig cvSysEGLInvalidConfigID = nullptr;
    constexpr size_t cvSysEGLMaxEGLConfigsNum = 256u;
    constexpr size_t cvSysEGLMaxEGLConfigAttributesNum = 64u;

    struct EGLSurfaceNativeData
    {
        EGLDisplay display;
        EGLNativeWindowType nativeWindow;
        EGLConfig fbConfig;
        EGLSurface surfaceHandle;
    };

    struct EGLRenderContextNativeData
    {
        EGLDisplay display;
        EGLSurface surfaceHandle;
        EGLRenderContext contextHandle;
    };

    struct EGLCoreDeviceNativeData
    {
        EGLDisplay display;
        EGLNativeWindowType androidNativeWindow;
        Version eglVersion;
    };

    void eglInitializeGLCoreDevice( SysGLCoreDevice & pDriver );
    EGLConfig eglChooseCoreEGLConfig( EGLDisplay pDisplay, const SysVisualConfig & pVisualConfig );
    void eglCreateSurface( SysGLSurface & pSurface, EGLDisplay pEGLDisplay, EGLNativeWindowType pWindow, EGLConfig pEGLConfig );
    void eglCreateCoreContext( SysGLRenderContext & pContext, const SysGLRenderContextCreateInfo & pCreateInfo );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__
