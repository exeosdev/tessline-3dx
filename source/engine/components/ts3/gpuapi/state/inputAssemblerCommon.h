
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	ts3GpaDeclareClassHandle( GPUBuffer );
	ts3GpaDeclareClassHandle( IAVertexFormatDescriptor );
	ts3GpaDeclareClassHandle( IAVertexStreamDescriptor );

	/// @brief
	using input_assembler_index_t = uint16;

	/// @brief
	enum : input_assembler_index_t
	{
		E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM ),

		E_IA_CONSTANT_VERTEX_ATTRIBUTE_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue,

		E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM ),

		E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue,
	};

	/// @brief
	inline constexpr bool ecIsIAVertexAttributeIndexValid( input_assembler_index_t pIndex )
	{
		return pIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM;
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexAttributeFlag( input_assembler_index_t pAttribIndex )
	{
		return ( pAttribIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM ) ? ( 1 << pAttribIndex ) : 0u;
	}

	/// @brief
	inline constexpr bool ecIsIAVertexStreamVBIndexValid( input_assembler_index_t pVBIndex )
	{
		return pVBIndex < E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM;
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexStreamVBFlag( input_assembler_index_t pVBIndex )
	{
		return ( pVBIndex < E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM ) ? ( 1 << pVBIndex ) : 0u;
	}

	/// @brief
	enum EIAVertexAttributeFlags : uint32
	{
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0  = ecMakeIAVertexAttributeFlag( 0 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1  = ecMakeIAVertexAttributeFlag( 1 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2  = ecMakeIAVertexAttributeFlag( 2 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3  = ecMakeIAVertexAttributeFlag( 3 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_4  = ecMakeIAVertexAttributeFlag( 4 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_5  = ecMakeIAVertexAttributeFlag( 5 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_6  = ecMakeIAVertexAttributeFlag( 6 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_7  = ecMakeIAVertexAttributeFlag( 7 ),
	};

	/// @brief
	enum EIAVertexStreamBindingFlags : uint32
	{
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_0  = ecMakeIAVertexStreamVBFlag( 0 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_1  = ecMakeIAVertexStreamVBFlag( 1 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_2  = ecMakeIAVertexStreamVBFlag( 2 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_3  = ecMakeIAVertexStreamVBFlag( 3 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_4  = ecMakeIAVertexStreamVBFlag( 4 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_5  = ecMakeIAVertexStreamVBFlag( 5 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_6  = ecMakeIAVertexStreamVBFlag( 6 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_7  = ecMakeIAVertexStreamVBFlag( 7 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER     = ecMakeIAVertexStreamVBFlag( E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM )
	};

	/// @brief Represents vertex input attribute.
	struct IAVertexInputAttributeInfo
	{
		input_assembler_index_t streamIndex{ E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED };

		uint16 instanceRate{ 0 };

		EVertexAttribFormat format{ EVertexAttribFormat::Undefined };

		gpu_memory_size_t relativeOffset{ CX_GPU_MEMORY_OFFSET_INVALID };

		void reset()
		{
			streamIndex = E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED;
			format = EVertexAttribFormat::Undefined;
		}

		TS3_FUNC_NO_DISCARD bool isActive() const
		{
			return ecIsIAVertexStreamVBIndexValid( streamIndex ) && ( format != EVertexAttribFormat::Undefined );
		}
	};

	/// Describes the binding of an index buffer, used for indexed draw calls. IndexBuffer is not required
	/// and the binding may be empty. In such case, any indexed draw call executed on a context with fail.
	struct IAIndexBufferBinding
	{
		/// A pointer to an existing buffer containing indices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT bit set.
		GPUBufferHandle bufferObject{ nullptr };

		/// A format of the data within the bound buffer's range.
		EIndexDataFormat dataFormat{ EIndexDataFormat::Undefined };

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset{ CX_GPU_MEMORY_OFFSET_INVALID };

		void reset()
		{
			bufferObject = nullptr;
			dataFormat = EIndexDataFormat::Undefined;
			dataOffset = CX_GPU_MEMORY_OFFSET_INVALID;
		}

		TS3_FUNC_NO_DISCARD bool isActive() const
		{
			return bufferObject && ( dataFormat != EIndexDataFormat::Undefined ) && ( dataOffset != CX_GPU_MEMORY_OFFSET_INVALID );
		}
	};

	/// @brief Describes the binding of one of the vertex buffers used to source vertices data during rendering.
	struct IAVertexBufferBinding
	{
		/// A pointer to an existing buffer containing vertices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT bit set.
		GPUBufferHandle bufferObject{ nullptr };

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset{ CX_GPU_MEMORY_OFFSET_INVALID };

		/// Size of a single vertex instance within the buffer. Basically, it's a size of a struct
		/// used to represent vertex sub-data stored in this buffer. Cannot be 0.
		gpu_memory_size_t dataStride{ 0 };

		void reset()
		{
			bufferObject = nullptr;
			dataOffset = CX_GPU_MEMORY_OFFSET_INVALID;
			dataStride = 0;
		}

		TS3_FUNC_NO_DISCARD bool isActive() const
		{
			return bufferObject && ( dataOffset != CX_GPU_MEMORY_OFFSET_INVALID ) && ( dataStride > 0 );
		}
	};

	/// @brief Describes a single, continuous range of vertex buffer binding points in the vertex stream.
	struct IAVertexBufferRange
	{
		/// First index of the vertex buffer binding this range defines.
		input_assembler_index_t firstIndex{ E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED };

		/// Length of this range, i.e. number of vertex buffers within the range.
		input_assembler_index_t length{ 0 };
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
