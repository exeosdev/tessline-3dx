
#include "eglCoreUtils.h"
#include <ts3/stdext/mapUtils.h>
#include <unordered_map>

namespace ts3::system
{

    Version EGLCoreAPI::queryRuntimeVersion()
    {
        Version resultVersion;

        int majorVersion = 0;
        glGetIntegerv( GL_MAJOR_VERSION, &majorVersion );
        resultVersion.major = static_cast<uint16>( majorVersion );

        int minorVersion = 0;
        glGetIntegerv( GL_MINOR_VERSION, &minorVersion );
        resultVersion.minor = static_cast<uint16>( minorVersion );

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
        return getMapValueOrDefault( errorStringMap, pError, "UNKNOWN_ERROR" );
    }

}
