
#include "../GL_prerequisites.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::gpuapi
{

	bool GLErrorHandler::checkLastResult()
	{
		GLenum errorCode = glGetError();
		return errorCode == GL_NO_ERROR;
	}

	bool GLErrorHandler::checkLastError( GLenum pErrorCode )
	{
		GLenum errorCode = glGetError();
		return ( pErrorCode != GL_NO_ERROR ) && ( errorCode == pErrorCode );
	}

	void GLErrorHandler::handleLastError()
	{
		GLenum errorCode = glGetError();
		if( errorCode != GL_NO_ERROR )
		{
			auto * errorMessageStr = translateErrorCode( errorCode );
			ts3DebugOutputFmt( "OpenGL API error: %s.", errorMessageStr );
			ts3DebugInterrupt();
		}
	}

	void GLErrorHandler::resetErrorQueue()
	{
		size_t errorsNum = 0;
		while( true )
		{
			GLenum errorCode = glGetError();
			if( errorCode == GL_NO_ERROR )
			{
				break;
			}
			auto * errorMessageStr = translateErrorCode( errorCode );
			ts3DebugOutputFmt( "OpenGL API error: %s.", errorMessageStr );
			++errorsNum;
		}
	}

	const char * GLErrorHandler::translateErrorCode( GLenum pError )
	{
		static const std::unordered_map<GLenum, const char *> errorStringMap =
		{
		#if( TS3GX_GL_PLATFORM_TYPE == TS3GX_GL_PLATFORM_TYPE_CORE )
			{ GL_STACK_OVERFLOW  , "GL_STACK_OVERFLOW"  },
			{ GL_STACK_UNDERFLOW , "GL_STACK_UNDERFLOW" },
		#endif
			{ GL_INVALID_ENUM                  , "GL_INVALID_ENUM"                  },
			{ GL_INVALID_VALUE                 , "GL_INVALID_VALUE"                 },
			{ GL_INVALID_OPERATION             , "GL_INVALID_OPERATION"             },
			{ GL_INVALID_FRAMEBUFFER_OPERATION , "GL_INVALID_FRAMEBUFFER_OPERATION" },
			{ GL_OUT_OF_MEMORY                 , "GL_OUT_OF_MEMORY"                 },
		};
		return ts3::getMapValueOrDefault( errorStringMap, pError, "UNKNOWN_ERROR" );
	}

}
