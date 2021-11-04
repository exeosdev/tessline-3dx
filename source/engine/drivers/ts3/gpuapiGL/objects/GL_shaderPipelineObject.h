
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PIPELINE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PIPELINE_OBJECT_H__

#include "../GL_prerequisites.h"

namespace ts3
{
namespace gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLShaderPipelineObject );
	ts3GLDeclareOpenGLObjectHandle( GLShaderProgramObject );

	class GLShaderPipelineObject : public GLObject
	{
	public:
		GLShaderPipelineObject( GLuint pHandle );
		virtual ~GLShaderPipelineObject();

		virtual bool release();
		virtual bool validateHandle() const;

		void attachProgram( GLShaderProgramObject & pProgram );

		void attachProgram( GLShaderProgramObject & pProgram, Bitmask<GLenum> pStageMask );

		void setActiveProgram( GLShaderProgramObject & pProgram );

		void resetActiveProgram();

		static GLShaderPipelineObjectHandle create();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_SHADER_PIPELINE_OBJECT_H__
