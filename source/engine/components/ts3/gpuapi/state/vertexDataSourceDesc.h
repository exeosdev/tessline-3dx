
#pragma once

#ifndef __TS3_GPUAPI_PIPELINE_STATE_DESC_VERTEX_STREAM_H__
#define __TS3_GPUAPI_PIPELINE_STATE_DESC_VERTEX_STREAM_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	class GPUBuffer;

	/// @brief Stores description of how an index buffer and vertex buffers are bound to the graphics pipeline.
	///
	/// This desc struct is used to create a VertexDataSourceBinding object, used to bind index/vertex buffers
	/// into the pipeline.
	struct VertexDataSourceBindingDesc
	{
		// Describes the binding of an index buffer, used for indexed draw calls. IndexBuffer is not required
		// and the binding may be empty. In such case, any indexed draw call executed on a context with fail.
		struct IndexBufferBindingDesc
		{
			// A pointer to an existing buffer containing indices data.
			// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT bit set.
			GPUBuffer * bufferObject = nullptr;
			// An offset from the beginning of the buffer's storage.
			gpu_memory_size_t dataOffset = 0;
			// A format of the data within the bound buffer's range.
			EIndexDataFormat format = EIndexDataFormat::Uint32;
		};

		// Describes the binding of one of the vertex buffers used to source vertices data during rendering.
		struct VertexBufferBindingDesc
		{
			// An index of one of the vertex input streams used to stream the data during the IA stage.
			// Must be between 0 and GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM.
			vertex_stream_index_t streamIndex = cxInvalidVertexStreamIndex;
			// A pointer to an existing buffer containing vertices data.
			// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT bit set.
			GPUBuffer * bufferObject = nullptr;
			// An offset from the beginning of the buffer's storage.
			gpu_memory_size_t dataOffset = 0;
			// Size of a single vertex instance within the buffer. Basically, it's a size of a struct
			// used to represent vertex subdata stored in this buffer. Cannot be 0.
			gpu_memory_size_t dataStride = 0;
		};

		using VertexBufferBindingDescArray = std::array<VertexBufferBindingDesc, GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM>;

		// Index buffer binding. IB is not required and it is not an error if this binding represents an empty
		// binding (buffer set to nullptr). In such case, any indexed draw call executed on a context with fail.
		IndexBufferBindingDesc indexBufferBindingDesc;

		// An array of vertex buffer bindings. This array does not have to be continuous (i.e. it may have gaps
		// in a form of empty bindings). THis enables better flexibility in case of auto-generated bindings.
		// Also, indices in this array have no connection whatsoever with Vertex Input Stream indices. In other
		// words, it is perfectly legal to define VB binding like that:
		// VertexDataSourceBindingDesc vbbDesc;
		// vbbDesc.vertexBufferBindingDescArray[0] = { 11, someBuffer, 0, sizeof( Vertex1 ) }; // binds whole someBuffer to stream 11
		// vbbDesc.vertexBufferBindingDescArray[6] = { 0, otherBuffer, 256, sizeof( Vertex2 ) }; // binds subrange of otherBuffer to stream 0
		VertexBufferBindingDescArray vertexBufferBindingDescArray;
	};

#define ts3gaIndexBufferBindingDesc( pIndexBuffer, pDataOffset, pFormat ) \
	ts3::gpuapi::VertexDataSourceBindingDesc::IndexBufferBindingDesc{ pIndexBuffer, pDataOffset, pFormat }

#define ts3gaVertexBufferBindingDesc( pStreamIndex, pVertexBuffer, pDataOffset, pDataStride ) \
	ts3::gpuapi::VertexDataSourceBindingDesc::VertexBufferBindingDesc{ pStreamIndex, pVertexBuffer, pDataOffset, pDataStride }


	/// @brief A generic, "default" representation of a processed VDS binding state, created from a desc struct above.
	///
	/// This struct in an API-agnostic representation of the binding state. It is usually used as an intermediate step
	/// in creating an API-specific binding. It's primary role is to provide common validation of a user input and some
	/// additional optimisations like building a list of VB binding ranges (otherwise every API supporting ranged binds
	/// would have to re-implement this process).
	/// This struct should probably never be created directly, but used internally by a VertexStreamStateObject.
	struct VertexDataSourceBinding
	{
		struct IndexBufferBinding
		{
			GPUBuffer * bufferObject = nullptr;
			gpu_memory_size_t dataOffset;
			EIndexDataFormat format;
		};

		struct VertexBufferBinding
		{
			GPUBuffer * bufferObject = nullptr;
			gpu_memory_size_t dataOffset;
			gpu_memory_size_t dataStride;
		};

		struct VertexStreamRange
		{
			uint32 firstIndex;
			uint32 length;
		};

		using VertexBufferBindingArray = std::array<VertexBufferBinding, GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM>;
		using VertexStreamIndexArray = std::vector<vertex_stream_index_t>;
		using VertexStreamRangeList = std::list<VertexStreamRange>;

		// Index buffer binding. Can be empty.
		IndexBufferBinding indexBufferBinding;

		// Number of active vertex buffers bindings.
		uint32 vertexBufferActiveBindingsNum;

		// Number of continuous ranges in the array of VB bindings.
		uint32 vertexBufferActiveRangesNum;

		// An ordered version of the VB binding array specified.
		// Index N in this array stores a binding for vertex stream N.
		VertexBufferBindingArray vertexBufferBindingArray;

		// A list (vector) of indices of all active vertex streams (i.e. those with a buffer set).
		VertexStreamIndexArray vertexStreamActiveIndexArray;

		// A list of all continuous ranges of VB bindings.
		// An example: if we used the following binding desc:
		// VertexDataSourceBindingDesc vbbDesc;
		// vbbDesc.vertexBufferBindingDescArray[0] = { 3, vBuffer1, 0, sizeof( Vertex1 ) };
		// vbbDesc.vertexBufferBindingDescArray[1] = { 4, vBuffer2, 0, sizeof( Vertex2 ) };
		// vbbDesc.vertexBufferBindingDescArray[2] = { 7, vBuffer3, 0, sizeof( Vertex3 ) };
		// we would get a list of two binding ranges: { 3, 2 } and { 7, 1 }.
		// Ranges are generated because many APIs expose an API to set continuous ranges of vertex buffers
		// as a single function call, which yields better performance (always reduce those API calls!).
		VertexStreamRangeList vertexStreamActiveRangeList;

		VertexDataSourceBinding() = default;

		VertexDataSourceBinding( const InitEmptyTag & )
		: vertexBufferActiveBindingsNum( 0 )
		{}

		explicit operator bool() const
		{
			return ( vertexBufferActiveBindingsNum > 0 ) && !vertexBufferBindingArray.empty();
		}
	};

	TS3_GPUAPI_API VertexDataSourceBinding createVertexDataSourceBinding( const VertexDataSourceBindingDesc & pBindingDesc );

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_PIPELINE_STATE_DESC_VERTEX_STREAM_H__
