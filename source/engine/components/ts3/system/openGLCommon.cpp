
#include "openGLCommon.h"
#include <ts3/stdext/stlHelperAlgo.h>
#include <sstream>
#include <unordered_map>

namespace ts3::system
{

	std::string OpenGLSystemVersionInfo::toString() const
	{
		std::ostringstream strStream;
		strStream << "[OpenGL Rendering Context Information]\n";
		strStream << "> API Version: " << apiVersion.major << "." << apiVersion.minor << "\n";
		strStream << "> Runtime Version: " << apiVersionStr << "\n";
		strStream << "> GLSL Version: " << glslVersionStr << "\n";
		strStream << "> Renderer ID: " << rendererName << "\n";
		strStream << "> Vendor Name: " << vendorName;
		return strStream.str();
	}

	Version OpenGLCoreAPI::queryRuntimeVersion()
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

	bool OpenGLCoreAPI::checkLastResult()
	{
		GLenum errorCode = glGetError();
		return errorCode == GL_NO_ERROR;
	}

	bool OpenGLCoreAPI::checkLastError( GLenum pErrorCode )
	{
		GLenum errorCode = glGetError();
		return ( pErrorCode != GL_NO_ERROR ) && ( errorCode == pErrorCode );
	}

	void OpenGLCoreAPI::handleLastError()
	{
		GLenum errorCode = glGetError();
		if( errorCode != GL_NO_ERROR )
		{
			auto * errorMessageStr = translateErrorCode( errorCode );
			ts3DebugOutputFmt( "OpenGL API error: %s.", errorMessageStr );
			ts3DebugInterrupt();
		}
	}

	void OpenGLCoreAPI::resetErrorQueue()
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

	const char * OpenGLCoreAPI::translateErrorCode( GLenum pError )
	{
		static const std::unordered_map<GLenum, const char *> errorStringMap =
		{
		#if( TS3_SYSTEM_GL_PLATFORM_TYPE == TS3_SYSTEM_GL_PLATFORM_TYPE_CORE )
			{ GL_STACK_OVERFLOW  , "GL_STACK_OVERFLOW"  },
			{ GL_STACK_UNDERFLOW , "GL_STACK_UNDERFLOW" },
		#endif
			{ GL_INVALID_ENUM				  , "GL_INVALID_ENUM"				  },
			{ GL_INVALID_VALUE				 , "GL_INVALID_VALUE"				 },
			{ GL_INVALID_OPERATION			 , "GL_INVALID_OPERATION"			 },
			{ GL_INVALID_FRAMEBUFFER_OPERATION , "GL_INVALID_FRAMEBUFFER_OPERATION" },
			{ GL_OUT_OF_MEMORY				 , "GL_OUT_OF_MEMORY"				 },
		};
		return stdx::getMapValueRefOrDefault( errorStringMap, pError, "UNKNOWN_ERROR" );
	}

} // namespace ts3::system
