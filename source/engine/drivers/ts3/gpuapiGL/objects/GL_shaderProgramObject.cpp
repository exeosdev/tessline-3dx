
#include "GL_shaderProgramObject.h"
#include "GL_shaderObject.h"
#include <ts3/stdext/byteArray.h>

namespace ts3::gpuapi
{

	GLShaderProgramObject::GLShaderProgramObject( GLuint pHandle, GLShaderProgramType pProgramType )
	: GLObject( GLObjectBaseType::ShaderProgram, pHandle )
	, mGLProgramType( pProgramType )
	{ }

	GLShaderProgramObject::~GLShaderProgramObject() = default;

	GLuint GLShaderProgramObject::queryCurrentShaderProgramBinding()
	{
		GLint shaderProgramHandle = 0;

		glGetIntegerv( GL_CURRENT_PROGRAM, &shaderProgramHandle );
		ts3OpenGLHandleLastError();

		return static_cast<GLuint>( shaderProgramHandle );
	}

	GLShaderProgramObjectHandle GLShaderProgramObject::create( GLShaderProgramType pProgramType )
	{
		auto programHandle = glCreateProgram();
		ts3OpenGLHandleLastError();

		if( pProgramType == GLShaderProgramType::Separable )
		{
			glProgramParameteri( programHandle, GL_PROGRAM_SEPARABLE, GL_TRUE );
			ts3OpenGLHandleLastError();
		}

		GLShaderProgramObjectHandle openglProgramObject{ new GLShaderProgramObject( programHandle, pProgramType ) };

		return openglProgramObject;
	}

	GLShaderProgramObjectHandle GLShaderProgramObject::createSeparableModule( GLShaderObject & pShader )
	{
		auto programObject = GLShaderProgramObject::create( GLShaderProgramType::Separable );
		programObject->attachShader( pShader );
		programObject->link();
		programObject->detachShader( pShader );
		return programObject;
	}

	GLShaderProgramObjectHandle GLShaderProgramObject::createSeparableModule( GLShaderObject & pShader, const GLShaderDataLayoutMap & pLayoutMap )
	{
		auto programObject = GLShaderProgramObject::create( GLShaderProgramType::Separable );
		programObject->attachShader( pShader );
		setProgramPreLinkBindings( *programObject, pLayoutMap );
		programObject->link();
		setProgramPostLinkBindings( *programObject, pLayoutMap );
		programObject->detachShader( pShader );
		return programObject;
	}

	void GLShaderProgramObject::setProgramPreLinkBindings( GLShaderProgramObject & pProgram, const GLShaderDataLayoutMap & pLayoutMap )
	{
		for( const auto & attributeLocation : pLayoutMap.attributeLocations )
		{
			glBindAttribLocation( pProgram.mGLHandle, attributeLocation.second, attributeLocation.first.data() );
			ts3OpenGLHandleLastError();
		}

		for( const auto & fragDataLocation : pLayoutMap.fragDataLocations )
		{
			glBindFragDataLocation( pProgram.mGLHandle, fragDataLocation.second, fragDataLocation.first.data() );
			ts3OpenGLHandleLastError();
		}
	}

	void GLShaderProgramObject::setProgramPostLinkBindings( GLShaderProgramObject & pProgram, const GLShaderDataLayoutMap & pLayoutMap )
	{
		for( const auto & samplerBinding : pLayoutMap.samplerBindings )
		{
			GLint samplerVariableLocation = glGetUniformLocation( pProgram.mGLHandle, samplerBinding.first.data() );
			ts3OpenGLHandleLastError();

			if( samplerVariableLocation != -1 )
			{
				glProgramUniform1i( pProgram.mGLHandle, samplerVariableLocation, samplerBinding.second );
				ts3OpenGLHandleLastError();
			}
		}

		for( const auto & uniformBlockBinding : pLayoutMap.uniformBlockBindings )
		{
			GLint blockIndex = glGetUniformBlockIndex( pProgram.mGLHandle, uniformBlockBinding.first.data() );
			ts3OpenGLHandleLastError();

			if( blockIndex != -1 )
			{
				glUniformBlockBinding( pProgram.mGLHandle, blockIndex, uniformBlockBinding.second );
				ts3OpenGLHandleLastError();
			}
		}
	}

	bool GLShaderProgramObject::release()
	{
		auto deleteStatus = queryParameter( GL_DELETE_STATUS );
		if( deleteStatus == GL_FALSE )
		{
			glDeleteProgram( mGLHandle );
			ts3OpenGLHandleLastError();

			return true;
		}

		return false;
	}

	bool GLShaderProgramObject::validateHandle() const
	{
		auto isProgram = glIsProgram( mGLHandle );
		ts3OpenGLHandleLastError();

		return isProgram != GL_FALSE;
	}

	void GLShaderProgramObject::attachShader( GLuint pShaderHandle )
	{
		glAttachShader( mGLHandle, pShaderHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderProgramObject::attachShader( const GLShaderObject & pShader )
	{
		glAttachShader( mGLHandle, pShader.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderProgramObject::detachAllShaders()
	{
		auto attachedShaders = getAttachedShaders();
		for( auto & shaderHandle : attachedShaders )
		{
			glDetachShader( mGLHandle, shaderHandle );
			ts3OpenGLHandleLastError();
		}
	}

	void GLShaderProgramObject::detachShader( GLuint pShaderHandle )
	{
		glDetachShader( mGLHandle, pShaderHandle );
		ts3OpenGLHandleLastError();
	}

	void GLShaderProgramObject::detachShader( const GLShaderObject & pShader )
	{
		glDetachShader( mGLHandle, pShader.mGLHandle );
		ts3OpenGLHandleLastError();
	}

	bool GLShaderProgramObject::link()
	{
		auto attachedShadersStageMask = queryShaderStageMask();

		glLinkProgram( mGLHandle );
		ts3OpenGLHandleLastError();

		auto linkStatus = queryParameter( GL_LINK_STATUS );
		if( linkStatus == GL_FALSE )
		{
			auto infoLog = getInfoLog();
			ts3DebugOutput( infoLog.data() );
			ts3DebugInterrupt();
			return false;
		}

		_linkedShadersStageMask = attachedShadersStageMask;

		return true;
	}

	bool GLShaderProgramObject::validate()
	{
		glValidateProgram( mGLHandle );
		ts3OpenGLHandleLastError();
		auto infoLog = getInfoLog();

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
		ts3OpenGLHandleLastError();
	}

	void GLShaderProgramObject::setSamplerTextureUnit( const char * pSamplerName, GLuint pTextureIndex )
	{
		TS3_DEBUG_CODE({
			auto linkResult = queryParameter( GL_LINK_STATUS );
			ts3DebugAssert( linkResult == GL_FALSE );
		});

		GLint samplerLocation = glGetUniformLocation( mGLHandle, pSamplerName );
		ts3OpenGLHandleLastError();

		glProgramUniform1i( mGLHandle, samplerLocation, pTextureIndex );
		ts3OpenGLHandleLastError();
	}

	void GLShaderProgramObject::setUniformBlockBinding( const char * pBlockName, GLuint pBinding )
	{
		TS3_DEBUG_CODE({
			auto linkResult = queryParameter( GL_LINK_STATUS );
			ts3DebugAssert( linkResult == GL_FALSE );
		});

		GLint uniformBlockIndex = glGetUniformBlockIndex( mGLHandle, pBlockName );
		ts3OpenGLHandleLastError();

		glUniformBlockBinding( mGLHandle, uniformBlockIndex, pBinding );
		ts3OpenGLHandleLastError();
	}

	GLint GLShaderProgramObject::queryParameter( GLenum pParameter ) const
	{
		GLint parameterValue = GL_INVALID_VALUE;

		glGetProgramiv( mGLHandle, pParameter, &parameterValue );
		ts3OpenGLHandleLastError();

		return parameterValue;
	}

	GLuint GLShaderProgramObject::queryVertexAttributeLocation( const char * pAttribName ) const
	{
		auto attribLocation = glGetAttribLocation( mGLHandle, reinterpret_cast<const GLchar *>( pAttribName ) );
		ts3OpenGLHandleLastError();

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
			ts3OpenGLHandleLastError();

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
			DynamicByteArray infoLogBuffer;
			infoLogBuffer.resize( infoLogLength );

			glGetProgramInfoLog( mGLHandle, static_cast<GLsizei>( infoLogLength ), nullptr, infoLogBuffer.dataAs<GLchar>() );
			ts3OpenGLHandleLastError();

			// Note: length returned by the GL includes null terminator!
			infoLog.assign( infoLogBuffer.dataAs<GLchar>(), infoLogLength - 1 );
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
			ts3OpenGLHandleLastError();

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
		auto infoLogLength = queryParameter( GL_INFO_LOG_LENGTH );
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

} // namespace ts3::gpuapi
