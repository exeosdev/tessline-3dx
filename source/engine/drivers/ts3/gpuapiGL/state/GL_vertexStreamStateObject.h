
#pragma once

#ifndef __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__

#include "GL_pipelineStateDesc.h"
#include "../objects/GL_bufferObject.h"
#include "../state/GL_pipelineStateDesc.h"
#include <ts3/gpuapi/state/vertexStreamStateObject.h>

namespace ts3
{
namespace gpuapi
{

	class GLVertexStreamStateObject : public VertexStreamStateObject
	{
	public:
		GLVertexDataSourceBinding const mGLVertexDataSourceBinding;

	public:
		GLVertexStreamStateObject( GLGPUDevice & pGPUDevice, GLVertexDataSourceBinding pVertexDataSourceBinding );

		static GpaHandle<GLVertexStreamStateObject> create( GLGPUDevice & pGPUDevice,
		                                                    const VertexStreamStateObjectCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_GLCOMMON_VERTEX_STREAM_STATE_OBJECT_H__
