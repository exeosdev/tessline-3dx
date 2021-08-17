
#include <ts3/system/internal/internalOpenGL.h>
#include <ts3/system/internal/internaltemContext.h>

#if( TS3_PCL_TARGET_SYSAPI == TS3_PCL_TARGET_SYSAPI_ANDROID )
namespace ts3
{

    bool GLImplProxy::nativeInitializeGLDriver( GLDriver & pDriver )
    {
        eglInitializeGLDriver( pDriver );
        pDriver.nativeData->androidNativeWindow = pDriver.systemContext->nativeData->androidNativeWindow;
        return true;
    }

    void GLImplProxy::nativeReleaseGLDriverInitState( GLDriver & pDriver )
    {}

    void GLImplProxy::nativeCreateGLSurface( GLDisplaySurface & pSurface, const GLDisplaySurfaceCreateInfo & pCreateInfo )
    {
        auto * driverNativeData = pSurface.driver->nativeData;

        EGLConfig fbConfig = eglChooseCoreEGLConfig( driverNativeData->display, pCreateInfo.visualConfig );
        EGLint fbConfigNativeVisualID = 0;
        // EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is guaranteed to be
        // accepted by ANativeWindow_setBuffersGeometry(). As soon as we retrieve an EGLConfig,
        // we can reconfigure the ANativeWindow buffers using the value of EGL_NATIVE_VISUAL_ID.
        if( ::eglGetConfigAttrib( driverNativeData->display, fbConfig, EGL_NATIVE_VISUAL_ID, &fbConfigNativeVisualID ) == EGL_FALSE )
        {
            throw 0;
        }

        auto * aNativeWindow = static_cast<ANativeWindow *>( driverNativeData->androidNativeWindow );
        if( ANativeWindow_setBuffersGeometry( aNativeWindow, 0, 0, fbConfigNativeVisualID ) < 0 )
        {
            throw 0;
        }

        eglCreateSurface( pSurface, driverNativeData->display, driverNativeData->androidNativeWindow, fbConfig );
    }

    void GLImplProxy::nativeCreateGLRenderContext( GLRenderContext & pContext, const GLRenderContextCreateInfo & pCreateInfo )
    {
        eglCreateCoreContext( pContext, pCreateInfo );
    }

    void GLImplProxy::nativeSwapBuffers( GLDisplaySurface & pSurface )
    {
        ::eglSwapBuffers( pSurface.nativeData->display, pSurface.nativeData->surfaceHandle );
    }

    void GLImplProxy::nativeBindContextForCurrentThread( GLRenderContext & pContext )
    {
        ::eglMakeCurrent( pContext.nativeData->display,
                          pContext.nativeData->surfaceHandle,
                          pContext.nativeData->surfaceHandle,
                          pContext.nativeData->contextHandle );
    }

}
#endif
