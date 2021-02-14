
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_RENDERBUFFER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_RENDERBUFFER_OBJECT_H__

#include "../GL_prerequisites.h"

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLRenderbufferObject );

	struct GLRenderbufferCreateInfo
	{
		math::Vec2u32 dimensions;
		GLenum internalFormat = 0;
		GLuint msaaLevel = 0;
	};

	class GLRenderbufferObject : public GLObject
	{
	public:
		math::Vec2u32 const mDimensions;
		GLenum const mGLInternalFormat;

		GLRenderbufferObject( GLuint pHandle, const GLRenderbufferCreateInfo & pGLCreateInfo );
		virtual ~GLRenderbufferObject();

		virtual bool release();
		virtual bool validateHandle() const;

		static GLRenderbufferObjectHandle create( const GLRenderbufferCreateInfo & pGLCreateInfo );

	private:
		bool initialize( const GLRenderbufferCreateInfo & pGLCreateInfo );
	};

}

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_RENDERBUFFER_OBJECT_H__
