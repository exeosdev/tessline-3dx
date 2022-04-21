
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__

#include "DX11_pipelineStateDesc.h"
#include "../state/DX11_pipelineStateDesc.h"
#include <ts3/gpuapi/state/vertexStreamStateObject.h>

namespace ts3
{
namespace gpuapi
{

	class DX11VertexStreamStateObject : public VertexStreamStateObject
	{
	public:
		DX11VertexDataSourceBinding const mDX11VertexDataSourceBinding;

	public:
		DX11VertexStreamStateObject( DX11GPUDevice & pGPUDevice,
		                             DX11VertexDataSourceBinding pDX11VertexDataSourceBinding );

		static GpaHandle<DX11VertexStreamStateObject> create( DX11GPUDevice & pGPUDevice,
		                                                      const VertexStreamStateObjectCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__
