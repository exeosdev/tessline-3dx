
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__

#include "GL_shaderCommon.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderObject );

	class GLShaderObject : public GLObject
	{
	public:
		GLenum const mGLShaderType;
		GLbitfield const mGLShaderStageMaskBit;

		GLShaderObject( GLuint pHandle, GLenum pGLShaderType, GLenum pGLShaderStageMaskBit );
		virtual ~GLShaderObject();

		virtual bool release();
		virtual bool validateHandle() const;

		bool compileSource( const void * pShaderSource, size_t pSourceLength );
		bool loadBinary( GLenum pFormat, const void * pBinary, size_t pBinarySize );

		void setDataLayoutMap( GLShaderDataLayoutMap pLayoutMap );

		TS3_ATTR_NO_DISCARD GLShaderDataLayoutMap * getDataLayoutMap() const noexcept;

		TS3_PCL_ATTR_FUNC_NO_DISCARD GLint queryParameter( GLenum pParameter ) const;

		TS3_PCL_ATTR_FUNC_NO_DISCARD std::string getInfoLog() const;
		TS3_PCL_ATTR_FUNC_NO_DISCARD std::string getSource() const;

		TS3_PCL_ATTR_FUNC_NO_DISCARD size_t getInfoLogLength() const;
		TS3_PCL_ATTR_FUNC_NO_DISCARD size_t getSourceLength() const;

		TS3_PCL_ATTR_FUNC_NO_DISCARD bool isInfoLogEmpty() const;
		TS3_PCL_ATTR_FUNC_NO_DISCARD bool isSourceEmpty() const;

		static bool checkBinaryCodeSupport();

		static bool checkBinaryFormatSupport( GLenum pFormat );

		static GLbitfield getStageMaskForEShaderType( GLenum pGLShaderType );

		static GLShaderObjectHandle create( GLenum pGLShaderType );
		static GLShaderObjectHandle createWithSource( GLenum pGLShaderType, const void * pShaderSource, size_t pSourceLength );

	private:
		std::unique_ptr<GLShaderDataLayoutMap> _dataLayoutMap;
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__
