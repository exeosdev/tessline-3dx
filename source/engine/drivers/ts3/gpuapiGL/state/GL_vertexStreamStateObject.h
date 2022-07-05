
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__

#include "GL_graphicsPipelineState.h"
#include "../objects/GL_bufferObject.h"
#include "../state/GL_graphicsPipelineState.h"
#include <ts3/gpuapi/state/vertexStreamStateObject.h>

namespace ts3::gpuapi
{

	class GLVertexStreamStateObject : public VertexStreamStateObject
	{
	public:
		GLVertexStreamBindingDescriptor const mGLVertexStreamBindingDescriptor;

	public:
		GLVertexStreamStateObject( GLGPUDevice & pGPUDevice, GLVertexStreamBindingDescriptor pVertexDataSourceBinding );

		static GpaHandle<GLVertexStreamStateObject> create( GLGPUDevice & pGPUDevice,
		                                                    const VertexStreamStateObjectCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__
