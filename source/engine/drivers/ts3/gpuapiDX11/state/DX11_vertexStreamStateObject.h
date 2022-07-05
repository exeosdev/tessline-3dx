
#pragma once

#ifndef __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__
#define __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__

#include "DX11_pipelineStateDesc.h"
#include "../state/DX11_pipelineStateDesc.h"
#include <ts3/gpuapi/state/vertexStreamStateObject.h>
#include <ts3/gpuapi/state/inputAssemblerCommon.h>

namespace ts3::gpuapi
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

	class DX11IABufferBindingDescriptor : public IABufferBindingDescriptor
	{
	public:
		struct DX11IndexBufferBinding
		{
			ID3D11Buffer * buffer;
			uint32 offset;
			DXGI_FORMAT format;
		};

		struct DX11VertexBufferBinding
		{
			ID3D11Buffer * bufferArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			UINT           offsetArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			UINT           strideArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
		};

		const DX11IndexBufferBinding mIndexBufferBinding;

		const DX11VertexBufferBinding mVertexBufferBinding;

	public:
		DX11IABufferBindingDescriptor( IABufferBindingCommonData pBindingData );

		virtual ~DX11IABufferBindingDescriptor() = default;
	};

	struct DX11VertexDataSourceBinding
	{
		struct DX11IndexBufferBinding
		{
			ID3D11Buffer * buffer;
			uint32 offset;
			DXGI_FORMAT format;
		};
		struct DX11VertexBufferBinding
		{
			ID3D11Buffer * bufferArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			UINT           offsetArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
			UINT           strideArray[E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM];
		};
		using DX11VertexStreamRange = VertexDataSourceBinding::VertexStreamRange;
		using DX11VertexStreamRangeList = VertexDataSourceBinding::VertexStreamRangeList;
		DX11IndexBufferBinding indexBufferBinding;
		uint32 vertexBufferActiveBindingsNum;
		DX11VertexBufferBinding vertexBufferBinding;
		DX11VertexStreamRangeList vertexStreamActiveRangeList;

		DX11VertexDataSourceBinding() = default;

		DX11VertexDataSourceBinding( const InitEmptyTag & )
				: vertexBufferActiveBindingsNum( 0 )
		{ }

		explicit operator bool() const
		{
			return ( vertexBufferActiveBindingsNum > 0 ) && !vertexStreamActiveRangeList.empty();
		}
	};

	DX11VertexDataSourceBinding createDX11VertexDataSourceBinding( const VertexDataSourceBinding & pBinding );

} // namespace ts3::gpuapi

#endif // __TS3DRIVER_GPUAPI_DX11_VERTEX_STREAM_STATE_OBJECT_H__
