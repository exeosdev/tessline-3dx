
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_ERROR_UTILS_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_ERROR_UTILS_H__

#include <ts3/core/exception.h>

#if !defined( TS3GX_GL_ENABLE_ERROR_CHECKS )
#  if( TS3_DEBUG || TS3GX_GL_ENABLE_ERROR_CHECKS_NON_DEBUG )
#    define TS3GX_GL_ENABLE_ERROR_CHECKS 1
#  else
#    define TS3GX_GL_ENABLE_ERROR_CHECKS 1
#  endif
#endif

namespace ts3::gpuapi
{

	class GLErrorHandler
	{
	public:
		static bool checkLastResult();

		static bool checkLastError( GLenum pErrorCode );

		static void handleLastError();

		static void resetErrorQueue();

		static const char * translateErrorCode( GLenum pError );
	};

}

#if( TS3GX_GL_ENABLE_ERROR_CHECKS )
#  define ts3GLCheckLastResult()             GLErrorHandler::checkLastResult()
#  define ts3GLCheckLastError( pErrorCode )  GLErrorHandler::checkLastError( pErrorCode )
#  define ts3GLHandleLastError()             GLErrorHandler::handleLastError()
#  define ts3GLResetErrorQueue()             GLErrorHandler::resetErrorQueue()
#else
#  define ts3GLCheckLastResult()
#  define ts3GLCheckLastError( pErrorCode )
#  define ts3GLHandleLastError()
#  define ts3GLResetErrorQueue()
#endif

#if( TS3GX_GL_ENABLE_ERROR_CHECKS )
#  define TS3_GL_CHECKED_API_CALL( pCall ) pCall; ts3GLHandleLastError()
#else
#  define TS3_GL_CHECKED_API_CALL( pCall ) pCall
#endif

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_ERROR_UTILS_H__
