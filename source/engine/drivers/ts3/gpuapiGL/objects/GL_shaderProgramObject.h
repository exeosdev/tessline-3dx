
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PROGRAM_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PROGRAM_OBJECT_H__

#include "GL_shaderCommon.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );

	enum class GLShaderProgramType : enum_default_value_t
	{
		Combined,
		Separable
	};

	class GLShaderProgramObject : public GLObject
	{
	public:
		GLShaderProgramType const mGLProgramType;

		GLShaderProgramObject( GLuint pHandle, GLShaderProgramType pProgramType );
		virtual ~GLShaderProgramObject();

		virtual bool release();
		virtual bool validateHandle() const;

		void attachShader( GLuint pShaderHandle );
		void attachShader( const GLShaderObject & pShader );

		void detachAllShaders();
		void detachShader( GLuint pShaderHandle );
		void detachShader( const GLShaderObject & pShader );

		bool link();
		bool validate();

		void setAttribLocation( const char * pAttribName, GLuint pLocation );
		void setSamplerTextureUnit( const char * pSamplerName, GLuint pTextureIndex );
		void setUniformBlockBinding( const char * pBlockName, GLuint pBinding );

		GLint queryParameter( GLenum pParameter ) const;
		GLuint queryVertexAttributeLocation( const char * pAttribName ) const;
		Bitmask<GLbitfield> queryShaderStageMask() const;

		std::string getInfoLog() const;
		std::vector<GLuint> getAttachedShaders() const;
		size_t getAttachedShadersNum() const;
		size_t getInfoLogLength() const;
		Bitmask<GLbitfield> getLinkedShaderStageMask() const;

		bool hasAttachedShaders() const;
		bool isInfoLogEmpty() const;

		static GLuint queryCurrentShaderProgramBinding();

		static GLShaderProgramObjectHandle create( GLShaderProgramType pProgramType );

		static GLShaderProgramObjectHandle createSeparableModule( GLShaderObject & pShader );
		static GLShaderProgramObjectHandle createSeparableModule( GLShaderObject & pShader, const GLShaderDataLayoutMap & pLayoutMap );

		static void setProgramPreLinkBindings( GLShaderProgramObject & pProgram, const GLShaderDataLayoutMap & pLayoutMap );
		static void setProgramPostLinkBindings( GLShaderProgramObject & pProgram, const GLShaderDataLayoutMap & pLayoutMap );

	private:
		GLbitfield _linkedShadersStageMask;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PROGRAM_OBJECT_H__
