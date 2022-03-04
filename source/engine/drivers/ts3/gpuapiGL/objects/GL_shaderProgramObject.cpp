
#include "GL_shaderProgramObject.h"
#include "GL_shaderObject.h"

namespace ts3
{
namespace gpuapi
{

	GLShaderProgramObject::GLShaderProgramObject( GLuint pHandle, GLShaderProgramType pProgramType )
	: GLObject( GLObjectBaseType::ShaderProgram, pHandle )
	, mGLProgramType( pProgramType )
	{ }

	GLShaderProgramObject::~GLShaderProgramObject() = default;

	GLShaderProgramObjectHandle GLShaderProgramObject::create( GLShaderProgramType pProgramType )
	{
		auto programHandle = glCreateProgram();
		ts3GLHandleLastError();

		if( pProgramType == GLShaderProgramType::Separable )
		{
			glProgramParameteri( programHandle, GL_PROGRAM_SEPARABLE, GL_TRUE );
			ts3GLHandleLastError();
		}

		GLShaderProgramObjectHandle openglProgramObject{ new GLShaderProgramObject( programHandle, pProgramType ) };

		return openglProgramObject;
	}

	GLShaderProgramObjectHandle GLShaderProgramObject::createSeparableModule( GLShaderObject & pShader )
	{
		auto programObject = GLShaderProgramObject::create( GLShaderProgramType::Separable );
		programObject->attachShader( pShader );
		programObject->link();
		programObject->validate();
		programObject->detachShader( pShader );
		return programObject;
	}

	bool GLShaderProgramObject::release()
	{
		auto deleteStatus = queryParameter( GL_DELETE_STATUS );
		if( deleteStatus == GL_FALSE )
		{
			glDeleteProgram( mGLHandle );
			ts3GLHandleLastError();

			return true;
		}

		return false;
	}

	bool GLShaderProgramObject::validateHandle() const
	{
		auto isProgram = glIsProgram( mGLHandle );
		ts3GLHandleLastError();

		return isProgram != GL_FALSE;
	}

	void GLShaderProgramObject::attachShader( GLuint pShaderHandle )
	{
		glAttachShader( mGLHandle, pShaderHandle );
		ts3GLHandleLastError();
	}

	void GLShaderProgramObject::attachShader( const GLShaderObject & pShader )
	{
		glAttachShader( mGLHandle, pShader.mGLHandle );
		ts3GLHandleLastError();
	}

	void GLShaderProgramObject::detachAllShaders()
	{
		auto attachedShaders = getAttachedShaders();
		for( auto & shaderHandle : attachedShaders )
		{
			glDetachShader( mGLHandle, shaderHandle );
			ts3GLHandleLastError();
		}
	}

	void GLShaderProgramObject::detachShader( GLuint pShaderHandle )
	{
		glDetachShader( mGLHandle, pShaderHandle );
		ts3GLHandleLastError();
	}

	void GLShaderProgramObject::detachShader( const GLShaderObject & pShader )
	{
		glDetachShader( mGLHandle, pShader.mGLHandle );
		ts3GLHandleLastError();
	}

	bool GLShaderProgramObject::link()
	{
		auto attachedShadersStageMask = queryShaderStageMask();

		glLinkProgram( mGLHandle );
		ts3GLHandleLastError();

		auto linkStatus = queryParameter( GL_LINK_STATUS );
		if( linkStatus == GL_FALSE )
		{
			auto infoLog = getInfoLog();
			// print info log
			ts3DebugInterrupt();
			return false;
		}

		_linkedShadersStageMask = attachedShadersStageMask;

		return true;
	}

	bool GLShaderProgramObject::validate()
	{
		glValidateProgram( mGLHandle );
		ts3GLHandleLastError();

		auto validateStatus = queryParameter( GL_VALIDATE_STATUS );
		if( validateStatus == GL_FALSE )
		{
			auto infoLog = getInfoLog();
			// print info log
			ts3DebugInterrupt();
			return false;
		}

		return true;
	}

	void GLShaderProgramObject::setAttribLocation( const char * pAttribName, GLuint pLocation )
	{
		TS3_DEBUG_CODE({
           auto linkResult = queryParameter( GL_LINK_STATUS );
           ts3DebugAssert( linkResult == GL_FALSE );
		});

		glBindAttribLocation( mGLHandle, pLocation, pAttribName );
		ts3GLHandleLastError();
	}

	void GLShaderProgramObject::setSamplerTextureUnit( const char * pSamplerName, GLuint pTextureIndex )
	{
		TS3_DEBUG_CODE({
			auto linkResult = queryParameter( GL_LINK_STATUS );
			ts3DebugAssert( linkResult == GL_FALSE );
		});

		GLint samplerLocation = glGetUniformLocation( mGLHandle, pSamplerName );
		ts3GLHandleLastError();

		glProgramUniform1i( mGLHandle, samplerLocation, pTextureIndex );
		ts3GLHandleLastError();
	}

	void GLShaderProgramObject::setUniformBlockBinding( const char * pBlockName, GLuint pBinding )
	{
		TS3_DEBUG_CODE({
			auto linkResult = queryParameter( GL_LINK_STATUS );
			ts3DebugAssert( linkResult == GL_FALSE );
		});

		GLint uniformBlockIndex = glGetUniformBlockIndex( mGLHandle, pBlockName );
		ts3GLHandleLastError();

		glUniformBlockBinding( mGLHandle, uniformBlockIndex, pBinding );
		ts3GLHandleLastError();
	}

	GLint GLShaderProgramObject::queryParameter( GLenum pParameter ) const
	{
		GLint parameterValue = GL_INVALID_VALUE;

		glGetProgramiv( mGLHandle, pParameter, &parameterValue );
		ts3GLHandleLastError();

		return parameterValue;
	}

	GLuint GLShaderProgramObject::queryVertexAttributeLocation( const char * pAttribName ) const
	{
		auto attribLocation = glGetAttribLocation( mGLHandle, reinterpret_cast<const GLchar *>( pAttribName ) );
		ts3GLHandleLastError();

		if( attribLocation == -1 )
		{
			// Warn?
			return cvGLInvalidProgramLocation;
		}

		return static_cast<GLuint>( attribLocation );
	}

	Bitmask<GLbitfield> GLShaderProgramObject::queryShaderStageMask() const
	{
		Bitmask<GLbitfield> shaderStageMask = 0;

		auto attachedShaders = getAttachedShaders();
		for( auto shaderHandle : attachedShaders )
		{
			GLint shaderType = 0;
			glGetShaderiv( shaderHandle, GL_SHADER_TYPE, &shaderType );
			ts3GLHandleLastError();

			auto shaderStageBit = GLShaderObject::getStageMaskForEShaderType( shaderType );
			shaderStageMask.set( shaderStageBit );
		}

		return shaderStageMask;
	}

	std::string GLShaderProgramObject::getInfoLog() const
	{
		std::string infoLog {};

		auto infoLogLength = getInfoLogLength();
		if ( infoLogLength > 0 )
		{
			DynamicMemoryBuffer infoLogBuffer;
			infoLogBuffer.resize( infoLogLength );

			glGetProgramInfoLog( mGLHandle, static_cast<GLsizei>( infoLogLength ), nullptr, infoLogBuffer.dataPtrAs<GLchar>() );
			ts3GLHandleLastError();

			// Note: length returned by the GL includes null terminator!
			infoLog.assign( infoLogBuffer.dataPtrAs<GLchar>(), infoLogLength - 1 );
		}

		return infoLog;
	}

	std::vector<GLuint> GLShaderProgramObject::getAttachedShaders() const
	{
		std::vector<GLuint> shaderArray;

		auto attachedShadersNum = queryParameter( GL_ATTACHED_SHADERS );
		if( attachedShadersNum > 0 )
		{
			shaderArray.resize( attachedShadersNum );

			GLsizei returnedShadersNum = 0;
			glGetAttachedShaders( mGLHandle, 64, &returnedShadersNum, shaderArray.data() );
			ts3GLHandleLastError();

			ts3DebugAssert( returnedShadersNum == attachedShadersNum );
		}

		return shaderArray;
	}

	size_t GLShaderProgramObject::getAttachedShadersNum() const
	{
		auto attachedShadersNum = queryParameter( GL_ATTACHED_SHADERS );
		return attachedShadersNum;
	}

	size_t GLShaderProgramObject::getInfoLogLength() const
	{
		auto infoLogLength = queryParameter( GL_ATTACHED_SHADERS );
		return infoLogLength;
	}

	Bitmask<GLbitfield> GLShaderProgramObject::getLinkedShaderStageMask() const
	{
		return _linkedShadersStageMask;
	}

	bool GLShaderProgramObject::hasAttachedShaders() const
	{
		auto attachedShadersNum = getAttachedShadersNum();
		return attachedShadersNum > 0;
	}

	bool GLShaderProgramObject::isInfoLogEmpty() const
	{
		auto infoLogLength = getInfoLogLength();
		return infoLogLength > 0;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
