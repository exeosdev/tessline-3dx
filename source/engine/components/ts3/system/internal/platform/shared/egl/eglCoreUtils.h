
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_EGL_OPENGL_H__

#include <ts3/system/openGLCommon.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace ts3::system
{

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
