
#include "GL_shaderObject.h"
#include <ts3/stdext/mapUtils.h>

namespace ts3::gpuapi
{

	GLShaderObject::GLShaderObject( GLuint pHandle, GLenum pGLEShaderType, GLenum pGLShaderStageMaskBit )
	: GLObject( GLObjectBaseType::Shader, pHandle )
	, mGLEShaderType( pGLEShaderType )
	, mGLShaderStageMaskBit( pGLShaderStageMaskBit )
	{}

	GLShaderObject::~GLShaderObject() = default;

	GLShaderObjectHandle GLShaderObject::create( GLenum pGLEShaderType )
	{
		auto shaderStageMaskBit = getStageMaskForEShaderType( pGLEShaderType );
		if( shaderStageMaskBit == cvGLInvalidValue )
		{
			return nullptr;
		}

		auto shaderHandle = glCreateShader( pGLEShaderType );
		ts3GLHandleLastError();

		GLShaderObjectHandle openglShaderObject{ new GLShaderObject( shaderHandle, pGLEShaderType, shaderStageMaskBit ) };

		return openglShaderObject;
	}

	GLShaderObjectHandle GLShaderObject::createWithSource( GLenum pGLEShaderType, const void * pSource, size_t pSourceLength )
	{
		if( !pSource || ( pSourceLength == 0 ) )
		{
			return nullptr;
		}

		auto shaderObject = create( pGLEShaderType );
		if( !shaderObject )
		{
			return nullptr;
		}

		if( !shaderObject->compileSource( pSource, pSourceLength ) )
		{
			return nullptr;
		}

		return shaderObject;
	}

	bool GLShaderObject::release()
	{
		auto deleteStatus = queryParameter( GL_DELETE_STATUS );
		if( deleteStatus == GL_FALSE )
		{
			glDeleteShader( mGLHandle );
			ts3GLHandleLastError();

			return true;
		}

		return false;
	}

	bool GLShaderObject::validateHandle() const
	{
		GLboolean checkResult = glIsShader( mGLHandle );
		return checkResult == GL_TRUE;
	}

	bool GLShaderObject::compileSource( const void * pSource, size_t pSourceLength )
	{
		const auto * sourceBuffer = reinterpret_cast<const GLchar*>( pSource );
		const auto sourceLength = static_cast<GLint>( pSourceLength );

		glShaderSource( mGLHandle, 1, &sourceBuffer, &sourceLength );
		ts3GLHandleLastError();

		glCompileShader( mGLHandle );
		ts3GLHandleLastError();

		auto compileStatus = queryParameter( GL_COMPILE_STATUS );
		if ( compileStatus != GL_TRUE )
		{
			auto infoLog = getInfoLog();
			// print info log
			ts3DebugInterrupt();
			return false;
		}

		return true;
	}

	bool GLShaderObject::loadBinary( GLenum pFormat, const void * pBinary, size_t pBinarySize )
	{
		if( !checkBinaryCodeSupport() )
		{
			return false;
		}

		glShaderBinary( 1, &mGLHandle, pFormat, pBinary, static_cast<GLsizei>( pBinarySize ) );
		ts3GLHandleLastError();

		return true;
	}

	GLint GLShaderObject::queryParameter( GLenum pParameter ) const
	{
		GLint parameterValue = GL_INVALID_VALUE;

		glGetShaderiv( mGLHandle, pParameter, &parameterValue );
		ts3GLHandleLastError();

		return parameterValue;
	}

	std::string GLShaderObject::getInfoLog() const
	{
		std::string infoLog {};

		auto infoLogLength = getInfoLogLength();
		if( infoLogLength > 0 )
		{
			ts3::DynamicMemoryBuffer infoLogBuffer;
			infoLogBuffer.resize( infoLogLength );

			glGetShaderInfoLog( mGLHandle, static_cast<GLsizei>( infoLogLength ), nullptr, infoLogBuffer.dataPtrAs<GLchar>() );
			ts3GLHandleLastError();

			// Note: length returned by the GL includes null terminator!
			infoLog.assign( infoLogBuffer.dataPtrAs<GLchar>(), infoLogLength - 1 );
		}

		return infoLog;
	}

	std::string GLShaderObject::getSource() const
	{
		std::string source {};

		auto sourceLength = getSourceLength();
		if (sourceLength > 0)
		{
			ts3::DynamicMemoryBuffer sourceBuffer;
			sourceBuffer.resize( sourceLength );

			glGetShaderSource( mGLHandle, static_cast<GLsizei>( sourceLength ), nullptr, sourceBuffer.dataPtrAs<GLchar>() );
			ts3GLHandleLastError();

			// Note: length returned by the GL includes null terminator!
			source.assign( sourceBuffer.dataPtrAs<GLchar>(), sourceLength - 1 );
		}

		return source;
	}

	size_t GLShaderObject::getInfoLogLength() const
	{
		auto infoLogLength = queryParameter( GL_INFO_LOG_LENGTH );
		return infoLogLength;
	}

	size_t GLShaderObject::getSourceLength() const
	{
		auto sourceLength = queryParameter( GL_SHADER_SOURCE_LENGTH );
		return sourceLength;
	}

	bool GLShaderObject::isInfoLogEmpty() const
	{
		auto infoLogLength = getInfoLogLength();
		return infoLogLength == 0;
	}

	bool GLShaderObject::isSourceEmpty() const
	{
		auto sourceLength = getSourceLength();
		return sourceLength == 0;
	}

	bool GLShaderObject::checkBinaryCodeSupport()
	{
		GLint shaderBinaryFormatsNum = 0;
		glGetIntegerv( GL_NUM_SHADER_BINARY_FORMATS, &shaderBinaryFormatsNum );
		ts3GLHandleLastError();

		return shaderBinaryFormatsNum > 0;
	}

	bool GLShaderObject::checkBinaryFormatSupport( GLenum pFormat)
	{
		GLint shaderBinaryFormatsNum = 0;
		glGetIntegerv( GL_NUM_SHADER_BINARY_FORMATS, &shaderBinaryFormatsNum );
		ts3GLHandleLastError();

		return shaderBinaryFormatsNum > 0;
	}

	GLbitfield GLShaderObject::getStageMaskForEShaderType( GLenum pGLEShaderType )
	{
		static const std::unordered_map<GLenum, GLbitfield> shaderTypeStageMaskMap =
		{
			{ GL_VERTEX_SHADER          , GL_VERTEX_SHADER_BIT          },
			{ GL_TESS_CONTROL_SHADER    , GL_TESS_CONTROL_SHADER_BIT    },
			{ GL_TESS_EVALUATION_SHADER , GL_TESS_EVALUATION_SHADER_BIT },
			{ GL_GEOMETRY_SHADER        , GL_GEOMETRY_SHADER_BIT        },
			{ GL_FRAGMENT_SHADER        , GL_FRAGMENT_SHADER_BIT        },
			{ GL_COMPUTE_SHADER         , GL_COMPUTE_SHADER_BIT         },
		};
		return ts3::getMapValueOrDefault( shaderTypeStageMaskMap, pGLEShaderType, cvGLInvalidValue );
	}

}
