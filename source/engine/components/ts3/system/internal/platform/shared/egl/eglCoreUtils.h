
#ifndef __TS3_SYSTEM_PLATFORM_OSAPI_EGL_CORE_UTILS_H__
#define __TS3_SYSTEM_PLATFORM_OSAPI_EGL_CORE_UTILS_H__

#include <ts3/system/openGLCommon.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace ts3::system
{

	struct EGLError
	{
	public:
		EGLenum errorCode;

	public:
		EGLError( EGLenum pErrorCode )
		: errorCode( pErrorCode )
		{}

		constexpr explicit operator bool() const
		{
			return errorCode != EGL_SUCCESS;
		}
	};

	class EGLCoreAPI
	{
	public:
		static Version queryRuntimeVersion();

		static bool checkLastResult();

		static bool checkLastError( EGLenum pErrorCode );

		static EGLError getLastError();

		static void handleLastError();

		static void resetErrorQueue();

		static const char * translateErrorCode( EGLenum pError );
	};

#define ts3EGLMakeErrorInfo( pEGLError ) \
	OpenGLErrorInfo( pEGLError, EGLCoreAPI::translateErrorCode( pEGLError ) )

#define ts3EGLGetLastErrorInfo() \
	ts3EGLMakeErrorInfo( ::eglGetError() )

#define ts3EGLThrowError( pErrorDesc ) \
	ts3ThrowDesc( E_EXC_SYSTEM_OPENGL_SUBSYS_EGL_ERROR, pErrorDesc )

#define ts3EGLThrowLastError() \
	ts3ThrowEx( E_EXC_SYSTEM_OPENGL_SUBSYS_EGL_ERROR, ts3EGLGetLastErrorInfo() )

} // namespace ts3::system

#if( TS3_SYSTEM_GL_ENABLE_ERROR_CHECKS )
#  define ts3EGLCheckLastResult()             EGLCoreAPI::checkLastResult()
#  define ts3EGLCheckLastError( pErrorCode )  EGLCoreAPI::checkLastError( pErrorCode )
#  define ts3EGLHandleLastError()             EGLCoreAPI::handleLastError()
#  define ts3EGLHandleLastError()             EGLCoreAPI::handleLastError()
#  define ts3EGLResetErrorQueue()             EGLCoreAPI::resetErrorQueue()
#else
#  define ts3EGLCheckLastResult()
#  define ts3EGLCheckLastError( pErrorCode )
#  define ts3EGLHandleLastError()
#  define ts3EGLResetErrorQueue()
#endif

#endif // __TS3_SYSTEM_PLATFORM_OSAPI_EGL_CORE_UTILS_H__
