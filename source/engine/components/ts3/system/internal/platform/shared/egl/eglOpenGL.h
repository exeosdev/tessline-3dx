
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

    class EGLCoreAPI
    {
    public:
        static Version queryRuntimeVersion();

        static bool checkLastResult();

        static bool checkLastError( GLenum pErrorCode );

        static void handleLastError();

        static void resetErrorQueue();

        static const char * translateErrorCode( GLenum pError );
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

#if( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  define ts3EGLCheckLastResult()             EGLCoreAPI::checkLastResult()
#  define ts3EGLCheckLastError( pErrorCode )  EGLCoreAPI::checkLastError( pErrorCode )
#  define ts3EGLHandleLastError()             EGLCoreAPI::handleLastError()
#  define ts3EGLResetErrorQueue()             EGLCoreAPI::resetErrorQueue()
#else
#  define ts3EGLCheckLastResult()
#  define ts3EGLCheckLastError( pErrorCode )
#  define ts3EGLHandleLastError()
#  define ts3EGLResetErrorQueue()
#endif

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__
