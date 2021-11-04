
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__

#include "../GL_prerequisites.h"
#include <ts3/stdext/memoryBuffer.h>

namespace ts3
{
namespace gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderObject );

	class GLShaderObject : public GLObject
	{
	public:
		GLenum const mGLEShaderType;
		GLbitfield const mGLShaderStageMaskBit;

		GLShaderObject( GLuint pHandle, GLenum pGLEShaderType, GLenum pGLShaderStageMaskBit );
		virtual ~GLShaderObject();

		virtual bool release();
		virtual bool validateHandle() const;

		bool compileSource( const void * pSource, size_t pSourceLength );
		bool loadBinary( GLenum pFormat, const void * pBinary, size_t pBinarySize );

		TS3_PCL_ATTR_NO_DISCARD GLint queryParameter( GLenum pParameter ) const;

		TS3_PCL_ATTR_NO_DISCARD std::string getInfoLog() const;
		TS3_PCL_ATTR_NO_DISCARD std::string getSource() const;

		TS3_PCL_ATTR_NO_DISCARD size_t getInfoLogLength() const;
		TS3_PCL_ATTR_NO_DISCARD size_t getSourceLength() const;

		TS3_PCL_ATTR_NO_DISCARD bool isInfoLogEmpty() const;
		TS3_PCL_ATTR_NO_DISCARD bool isSourceEmpty() const;

		static bool checkBinaryCodeSupport();

		static bool checkBinaryFormatSupport( GLenum pFormat );

		static GLbitfield getStageMaskForEShaderType( GLenum pGLEShaderType );

		static GLShaderObjectHandle create( GLenum pGLEShaderType );
		static GLShaderObjectHandle createWithSource( GLenum pGLEShaderType, const void * pSource, size_t pSourceLength );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_OBJECT_H__
