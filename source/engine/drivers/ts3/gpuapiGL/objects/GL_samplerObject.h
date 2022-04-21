
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_OBJECT_H__

#include "../GL_prerequisites.h"

namespace ts3::gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLSamplerObject );

	struct GLSamplerState
	{
		GLenum addressModeS;
		GLenum addressModeT;
		GLenum addressModeR;
		GLint anisotropyLevel;
		GLenum magFilter;
		GLenum minFilter;
		GLenum textureCompareMode;
		GLenum textureCompareFunc;
		math::RGBAColorR32Norm borderColor;
		std::pair<GLfloat, GLfloat> mipLODRange;
	};

	class GLSamplerObject : public GLObject
	{
	public:
		GLSamplerObject( GLuint pHandle );
		virtual ~GLSamplerObject();

		virtual bool release();
		virtual bool validateHandle() const;

		bool setSamplerState( const GLSamplerState & pSamplerState );

		static GLSamplerObjectHandle create( const GLSamplerState & pSamplerState );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SAMPLER_OBJECT_H__
