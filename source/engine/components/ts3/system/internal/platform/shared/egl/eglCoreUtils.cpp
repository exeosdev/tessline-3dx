
#include "eglCoreUtils.h"
#include <ts3/stdext/stlHelperAlgo.h>
#include <ts3/stdext/stringExt.h>
#include <ts3/stdext/stringUtils.h>
#include <unordered_map>

namespace ts3::system
{

    Version EGLCoreAPI::queryRuntimeVersion()
    {
        EGLDisplay eDisplay = ::eglGetCurrentDisplay();
        std::string versionString = ::eglQueryString( eDisplay, EGL_VERSION );

        auto dotPos = versionString.find_first_of( '.' );
        auto endPos = versionString.find_first_not_of( "0123456789", dotPos );

        Version resultVersion;

        auto majorVersion = fromString<int32>( versionString.substr( 0, dotPos ) );
        resultVersion.major = static_cast<uint16>( majorVersion.first );

        auto minorVersion = fromString<int32>( versionString.substr( dotPos + 1, endPos ) );
        resultVersion.minor = static_cast<uint16>( minorVersion.first );

        return resultVersion;
    }

    bool EGLCoreAPI::checkLastResult()
    {
        GLenum errorCode = ::eglGetError();
        return errorCode == EGL_SUCCESS;
    }

    bool EGLCoreAPI::checkLastError( GLenum pErrorCode )
    {
        GLenum errorCode = ::eglGetError();
        return ( pErrorCode != EGL_SUCCESS ) && ( errorCode == pErrorCode );
    }

    EGLError EGLCoreAPI::getLastError()
    {
        GLenum errorCode = ::eglGetError();
        return EGLError{ errorCode };
    }

    void EGLCoreAPI::handleLastError()
    {
        GLenum errorCode = ::eglGetError();
        if( errorCode != EGL_SUCCESS )
        {
            auto * errorMessageStr = translateErrorCode( errorCode );
            ts3DebugOutputFmt( "OpenGL API error: %s.", errorMessageStr );
             ts3DebugInterrupt();
        }
    }

    void EGLCoreAPI::resetErrorQueue()
    {
        size_t errorsNum = 0;
        while( true )
        {
            GLenum errorCode = ::eglGetError();
            if( errorCode == EGL_SUCCESS )
            {
                break;
            }
            auto * errorMessageStr = translateErrorCode( errorCode );
            ts3DebugOutputFmt( "OpenGL API error: %s.", errorMessageStr );
            ++errorsNum;
        }
    }

    const char * EGLCoreAPI::translateErrorCode( GLenum pError )
    {
		switch( pError )
        {
		case EGL_SUCCESS             : return "EGL_SUCCESS"             ;
		case EGL_NOT_INITIALIZED     : return "EGL_NOT_INITIALIZED"     ;
		case EGL_BAD_ACCESS          : return "EGL_BAD_ACCESS"          ;
		case EGL_BAD_ALLOC           : return "EGL_BAD_ALLOC"           ;
		case EGL_BAD_ATTRIBUTE       : return "EGL_BAD_ATTRIBUTE"       ;
		case EGL_BAD_CONFIG          : return "EGL_BAD_CONFIG"          ;
		case EGL_BAD_CONTEXT         : return "EGL_BAD_CONTEXT"         ;
		case EGL_BAD_CURRENT_SURFACE : return "EGL_BAD_CURRENT_SURFACE" ;
		case EGL_BAD_DISPLAY         : return "EGL_BAD_DISPLAY"         ;
		case EGL_BAD_MATCH           : return "EGL_BAD_MATCH"           ;
		case EGL_BAD_NATIVE_PIXMAP   : return "EGL_BAD_NATIVE_PIXMAP"   ;
		case EGL_BAD_NATIVE_WINDOW   : return "EGL_BAD_NATIVE_WINDOW"   ;
		case EGL_BAD_PARAMETER       : return "EGL_BAD_PARAMETER"       ;
		case EGL_BAD_SURFACE         : return "EGL_BAD_SURFACE"         ;
		case EGL_CONTEXT_LOST        : return "EGL_CONTEXT_LOST"        ;
		}
		return "<UNKNOWN>";
    }

}
