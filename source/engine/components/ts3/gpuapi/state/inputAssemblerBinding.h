
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	class GPUBuffer;

	enum EVertexStreamSlot : vertex_stream_index_t
	{
		E_VERTEX_STREAM_SLOT_VB_0,
		E_VERTEX_STREAM_SLOT_VB_1,
		E_VERTEX_STREAM_SLOT_VB_2,
		E_VERTEX_STREAM_SLOT_VB_3,
		E_VERTEX_STREAM_SLOT_VB_4,
		E_VERTEX_STREAM_SLOT_VB_5,
		E_VERTEX_STREAM_SLOT_VB_6,
		E_VERTEX_STREAM_SLOT_VB_7,
		E_VERTEX_STREAM_SLOT_VB_8,
		E_VERTEX_STREAM_SLOT_VB_9,
		E_VERTEX_STREAM_SLOT_VB_10,
		E_VERTEX_STREAM_SLOT_VB_11,
		E_VERTEX_STREAM_SLOT_VB_12,
		E_VERTEX_STREAM_SLOT_VB_13,
		E_VERTEX_STREAM_SLOT_VB_14,
		E_VERTEX_STREAM_SLOT_VB_15,
		E_VERTEX_STREAM_SLOT_IB,
	};

	enum EVertexStreamBindingFlags : uint32
	{
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_0_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_0,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_1_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_1,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_2_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_2,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_3_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_3,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_4_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_4,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_5_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_5,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_6_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_6,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_7_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_7,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_8_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_8,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_9_BIT  = 1u << E_VERTEX_STREAM_SLOT_VB_9,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_10_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_10,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_11_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_11,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_12_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_12,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_13_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_13,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_14_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_14,
		E_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_15_BIT = 1u << E_VERTEX_STREAM_SLOT_VB_15,
		E_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT     = 1u << E_VERTEX_STREAM_SLOT_IB,
		E_IA_BUFFER_MASK_VERTEX_BUFFER_ALL                = 0xFFFF,
		E_IA_BUFFER_MASK_ALL                              = E_IA_BUFFER_MASK_VERTEX_BUFFER_ALL | E_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT
	};

	/// @brief
	struct VertexStreamBufferRange
	{
		/// First index of the vertex buffer binding this range defines.
		vertex_stream_index_t firstIndex = CX_VERTEX_STREAM_INDEX_INVALID;

		/// Length of this range, i.e. number of vertex buffers within the range.
		vertex_stream_index_t length = 0;
	};

	/// Describes the binding of an index buffer, used for indexed draw calls. IndexBuffer is not required
	/// and the binding may be empty. In such case, any indexed draw call executed on a context with fail.
	struct IndexBufferBindingDesc
	{
		/// A pointer to an existing buffer containing indices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT bit set.
		GPUBuffer * bufferObject = nullptr;

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset = 0;

		/// A format of the data within the bound buffer's range.
		EIndexDataFormat format = EIndexDataFormat::Uint32;

		explicit operator bool() const
		{
			return bufferObject != nullptr;
		}
	};

	/// @brief Describes the binding of one of the vertex buffers used to source vertices data during rendering.
	struct VertexBufferBindingDesc
	{
		/// An index of one of the vertex input streams used to stream the data during the IA stage.
		/// Must be between 0 and E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM.
		vertex_stream_index_t streamIndex = CX_VERTEX_STREAM_INDEX_INVALID;

		/// A pointer to an existing buffer containing vertices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT bit set.
		GPUBuffer * bufferObject = nullptr;

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset = 0;

		/// Size of a single vertex instance within the buffer. Basically, it's a size of a struct
		/// used to represent vertex sub-data stored in this buffer. Cannot be 0.
		gpu_memory_size_t dataStride = 0;

		explicit operator bool() const
		{
			return bufferObject != nullptr;
		}
	};

	using VertexBufferBindingDescArray = std::array<VertexBufferBindingDesc, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_INPUT_STREAMS_NUM>;

	/// @brief
	struct VertexStreamBufferBindingDesc
	{
		IndexBufferBindingDesc indexBufferBinding;

		VertexBufferBindingDescArray vertexBufferBindings;
	};

	/// @brief
	class VertexStreamDescriptor
	{
	public:
		using VertexBufferIndexArray = std::vector<vertex_stream_index_t>;
		using VertexBufferRangeArray = std::vector<VertexStreamBufferRange>;
		const Bitmask<EVertexStreamBindingFlags> mActiveBindingsMask;
		const uint16 mVertexBufferActiveBindingsNum;
		const uint16 mVertexBufferActiveRangesNum;
		const VertexBufferIndexArray mVertexBuffer≥awActiveIndices;
		const VertexBufferRangeArray mVertexBufferActiveRanges;

	public:
		explicit VertexStreamDescriptor( Bitmask<EVertexStreamBindingFlags> pActiveBindingsMask,
										 VertexBufferIndexArray pVertexBufferActiveIndices,
		                                 VertexBufferRangeArray pVertexBufferActiveRanges );

		virtual ~VertexStreamDescriptor() = default;

		/// @brief
		TS3_FUNC_NO_DISCARD bool empty() const
		{
			return ( mVertexBufferActiveBindingsNum == 0 ) || !mActiveBindingsMask.isSetAnyOf( E_IA_BUFFER_MASK_VERTEX_BUFFER_ALL );
		}

		/// @brief
		TS3_FUNC_NO_DISCARD bool hasIndexBuffer() const
		{
			return mActiveBindingsMask.isSet( E_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT );
		}

	protected:
		struct InternalBindingDesc
		{
			IndexBufferBindingDesc indexBufferBinding;
			VertexBufferBindingDescArray vertexBufferOrderedBindings;
			Bitmask<EVertexStreamBindingFlags> activeBindingsMask;
			VertexBufferIndexArray vertexBufferActiveBindings;
			VertexBufferRangeArray vertexBufferActiveRanges;

			InternalBindingDesc() = default;

			InternalBindingDesc( std::nullptr_t )
			: activeBindingsMask( 0 )
			{}

			explicit operator bool() const
			{
				return !vertexBufferActiveBindings.empty() && activeBindingsMask.isSetAnyOf( E_IA_BUFFER_MASK_VERTEX_BUFFER_ALL );
			}
		};

		/// @brief
		TS3_FUNC_NO_DISCARD static InternalBindingDesc createInternalBindingDesc( const VertexStreamBufferBindingDesc & pBindingDesc );
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__
