
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_H__

#include "../GL_prerequisites.h"

namespace ts3
{
namespace gpuapi
{

	ts3GLDeclareOpenGLObjectHandle( GLVertexArrayObject );

	class GLVertexArrayObject : public GLObject
	{
	public:
		GLVertexArrayObject( GLuint pHandle );
		virtual ~GLVertexArrayObject();

		virtual bool release();
		virtual bool validateHandle() const;

		static GLVertexArrayObjectHandle create();
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_ARRAY_OBJECT_H__
