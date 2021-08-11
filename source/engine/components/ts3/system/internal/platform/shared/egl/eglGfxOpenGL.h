
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__

#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace ts3
{

    constexpr EGLConfig cvEGLInvalidConfigID = nullptr;
    constexpr size_t cvEGLMaxEGLConfigsNum = 256u;
    constexpr size_t cvEGLMaxEGLConfigAttributesNum = 64u;

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

    struct EGLDriverNativeData
    {
        EGLDisplay display;
        EGLNativeWindowType androidNativeWindow;
        Version eglVersion;
    };

    void eglInitializeGLDriver( GLDriver & pDriver );
    EGLConfig eglChooseCoreEGLConfig( EGLDisplay pDisplay, const VisualConfig & pVisualConfig );
    void eglCreateSurface( GLDisplaySurface & pSurface, EGLDisplay pEGLDisplay, EGLNativeWindowType pWindow, EGLConfig pEGLConfig );
    void eglCreateCoreContext( GLRenderContext & pContext, const GLRenderContextCreateInfo & pCreateInfo );

}

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_EGL_GFX_OPENGL_H__
