
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
        static const std::unordered_map<GLenum, const char *> errorStringMap =
        {
            { EGL_SUCCESS             , "EGL_SUCCESS"             },
            { EGL_NOT_INITIALIZED     , "EGL_NOT_INITIALIZED"     },
            { EGL_BAD_ACCESS          , "EGL_BAD_ACCESS"          },
            { EGL_BAD_ALLOC           , "EGL_BAD_ALLOC"           },
            { EGL_BAD_ATTRIBUTE       , "EGL_BAD_ATTRIBUTE"       },
            { EGL_BAD_CONFIG          , "EGL_BAD_CONFIG"          },
            { EGL_BAD_CONTEXT         , "EGL_BAD_CONTEXT"         },
            { EGL_BAD_CURRENT_SURFACE , "EGL_BAD_CURRENT_SURFACE" },
            { EGL_BAD_DISPLAY         , "EGL_BAD_DISPLAY"         },
            { EGL_BAD_MATCH           , "EGL_BAD_MATCH"           },
            { EGL_BAD_NATIVE_PIXMAP   , "EGL_BAD_NATIVE_PIXMAP"   },
            { EGL_BAD_NATIVE_WINDOW   , "EGL_BAD_NATIVE_WINDOW"   },
            { EGL_BAD_PARAMETER       , "EGL_BAD_PARAMETER"       },
            { EGL_BAD_SURFACE         , "EGL_BAD_SURFACE"         },
            { EGL_CONTEXT_LOST        , "EGL_CONTEXT_LOST"        },
        };
        return stdx::getMapValueRefOrDefault( errorStringMap, pError, "UNKNOWN_ERROR" );
    }

}
