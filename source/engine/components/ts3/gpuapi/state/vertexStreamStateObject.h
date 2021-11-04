
#pragma once

#ifndef __TS3_GPUAPI_VERTEX_STREAM_STATE_OBJECT_H__
#define __TS3_GPUAPI_VERTEX_STREAM_STATE_OBJECT_H__

#include "gpuStateObject.h"
#include "vertexDataSourceDesc.h"

namespace ts3
{
namespace gpuapi
{

	ts3DeclareClassHandle( VertexStreamStateObject );
	ts3DeclareClassHandle( CommonVertexStreamStateObject );

	struct VertexStreamStateObjectCreateInfo
	{
		VertexDataSourceBindingDesc vertexDataSourceBindingDesc;
	};

	class TS3_GPUAPI_CLASS VertexStreamStateObject : public GPUStateObject
	{
	public:
		VertexStreamStateObject( GPUDevice & pGPUDevice );
		virtual ~VertexStreamStateObject();

	protected:
		struct CommonSSOState
		{
			VertexDataSourceBinding vertexDataSourceBinding;
		};

		static bool createCommonSSOState( const VertexStreamStateObjectCreateInfo & pCreateInfo, CommonSSOState & pOutputState );
	};

	class TS3_GPUAPI_CLASS CommonVertexStreamStateObject : public VertexStreamStateObject
	{
	public:
		VertexDataSourceBinding const mVertexDataSourceBinding;

		CommonVertexStreamStateObject( GPUDevice & pGPUDevice, VertexDataSourceBinding pVertexDataSourceBinding );
		virtual ~CommonVertexStreamStateObject();

		static CommonVertexStreamStateObjectHandle create( GPUDevice & pGPUDevice, const VertexStreamStateObjectCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3_GPUAPI_VERTEX_STREAM_STATE_OBJECT_H__
