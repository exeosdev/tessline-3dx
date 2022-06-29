
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	ts3DeclareClassHandle( GPUBuffer );

	/// @brief
	using input_assembler_index_t = uint16;

	/// @brief
	enum EIAConstants : input_assembler_index_t
	{
		E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM ),

		E_IA_CONSTANT_VERTEX_ATTRIBUTE_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue,

		E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM ),

		E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue,
	};

	/// @brief
	enum class EIAVertexAttributeIndex : input_assembler_index_t
	{
		Attrib0 ,
		Attrib1 ,
		Attrib2 ,
		Attrib3 ,
		Attrib4 ,
		Attrib5 ,
		Attrib6 ,
		Attrib7 ,
		Attrib8 ,
		Attrib9 ,
		Attrib10,
		Attrib11,
		Attrib12,
		Attrib13,
		Attrib14,
		Attrib15,
		Undefined = E_IA_CONSTANT_VERTEX_ATTRIBUTE_INDEX_UNDEFINED
	};

	/// @brief
	inline constexpr EIAVertexAttributeIndex ecMakeIAVertexAttributeIndex( input_assembler_index_t pIndex )
	{
		return ( pIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM ) ?
		       static_cast<EIAVertexAttributeIndex>( pIndex ) :
		       EIAVertexAttributeIndex::Undefined;
	}

	/// @brief
	inline constexpr bool ecIsIAVertexAttributeIndexValid( input_assembler_index_t pIndex )
	{
		return pIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM;
	}

	/// @brief
	inline constexpr bool ecIsIAVertexAttributeIndexValid( EIAVertexAttributeIndex pIndex )
	{
		return ecIsIAVertexAttributeIndexValid( static_cast<input_assembler_index_t>( pIndex ) );
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexAttributeFlag( input_assembler_index_t pAttribIndex )
	{
		return ( pAttribIndex < E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM ) ? ( 1 << pAttribIndex ) : 0u;
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex pAttribIndex )
	{
		return ecMakeIAVertexAttributeFlag( static_cast<uint32>( pAttribIndex ) );
	}

	/// @brief
	enum class EIAVertexStreamVBIndex : input_assembler_index_t
	{
		Slot0 ,
		Slot1 ,
		Slot2 ,
		Slot3 ,
		Slot4 ,
		Slot5 ,
		Slot6 ,
		Slot7 ,
		Slot8 ,
		Slot9 ,
		Slot10,
		Slot11,
		Slot12,
		Slot13,
		Slot14,
		Slot15,
		Undefined = E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED
	};

	/// @brief
	inline constexpr EIAVertexStreamVBIndex ecMakeIAVertexStreamVBIndex( int64 pVBIndex )
	{
		return ( pVBIndex < E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM ) ?
		       static_cast<EIAVertexStreamVBIndex>( pVBIndex ) :
		       EIAVertexStreamVBIndex::Undefined;
	}

	/// @brief
	inline constexpr bool ecIsIAVertexStreamVBIndexValid( input_assembler_index_t pVBIndex )
	{
		return pVBIndex < E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM;
	}

	/// @brief
	inline constexpr bool ecIsIAVertexStreamVBIndexValid( EIAVertexStreamVBIndex pVBIndex )
	{
		return ecIsIAVertexStreamVBIndexValid( static_cast<input_assembler_index_t>( pVBIndex ) );
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexStreamVBFlag( input_assembler_index_t pVBIndex )
	{
		return ( pVBIndex < E_IA_CONSTANT_MAX_VERTEX_STREAMS_NUM ) ? ( 1 << pVBIndex ) : 0u;
	}

	/// @brief
	inline constexpr uint32 ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex pVBIndex )
	{
		return ecMakeIAVertexStreamVBFlag( static_cast<input_assembler_index_t>( pVBIndex ) );
	}

	/// @brief
	enum EIAVertexAttributeFlags : uint32
	{
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib0 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib1 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib2 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib3 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_4  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib4 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_5  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib5 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_6  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib6 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_7  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib7 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_8  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib8 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_9  = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib9 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_10 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib10 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_11 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib11 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_12 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib12 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_13 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib13 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_14 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib14 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_15 = ecMakeIAVertexAttributeFlag( EIAVertexAttributeIndex::Attrib15 ),
	};

	/// @brief
	enum EIAVertexStreamBindingFlags : uint32
	{
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_0  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot0 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_1  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot1 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_2  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot2 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_3  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot3 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_4  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot4 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_5  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot5 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_6  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot6 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_7  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot7 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_8  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot8 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_9  = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot9 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_10 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot10 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_11 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot11 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_12 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot12 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_13 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot13 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_14 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot14 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_15 = ecMakeIAVertexStreamVBFlag( EIAVertexStreamVBIndex::Slot15 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER     = E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_15 << 1
	};

	/// @brief
	struct IAVertexInputAttributeInfo
	{
		EIAVertexStreamVBIndex streamIndex{ EIAVertexStreamVBIndex::Undefined };

		EVertexAttribFormat format{ EVertexAttribFormat::UNKNOWN };

		gpu_memory_size_t relativeOffset{ CX_GPU_MEMORY_OFFSET_INVALID };

		uint32 instanceRate{ 0 };

		constexpr explicit operator bool() const
		{
			return isValid();
		}

		void reset()
		{
			streamIndex = EIAVertexStreamVBIndex::Undefined;
			format = EVertexAttribFormat::UNKNOWN;
			relativeOffset = CX_GPU_MEMORY_OFFSET_INVALID;
			instanceRate = 0;
		}

		TS3_FUNC_NO_DISCARD bool isActive() const
		{
			return ecIsIAVertexStreamVBIndexValid( streamIndex ) &&
			       ( format != EVertexAttribFormat::UNKNOWN ) &&
			       ( relativeOffset != CX_GPU_MEMORY_OFFSET_INVALID );
		}

		TS3_FUNC_NO_DISCARD bool isEmpty() const
		{
			return streamIndex == EIAVertexStreamVBIndex::Undefined;
		}

		TS3_FUNC_NO_DISCARD bool isValid() const
		{
			return isActive() || isEmpty();
		}
	};

	/// Describes the binding of an index buffer, used for indexed draw calls. IndexBuffer is not required
	/// and the binding may be empty. In such case, any indexed draw call executed on a context with fail.
	struct IAIndexBufferBinding
	{
		/// A pointer to an existing buffer containing indices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT bit set.
		GPUBufferHandle bufferObject{ nullptr };

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset{ CX_GPU_MEMORY_OFFSET_INVALID };

		/// A format of the data within the bound buffer's range.
		EIndexDataFormat format{ EIndexDataFormat::Undefined };

		explicit operator bool() const
		{
			return isValid();
		}

		void reset()
		{
			bufferObject = nullptr;
			dataOffset = CX_GPU_MEMORY_OFFSET_INVALID;
			format = EIndexDataFormat::Undefined;
		}

		TS3_FUNC_NO_DISCARD bool isActive() const
		{
			return bufferObject && ( dataOffset != CX_GPU_MEMORY_OFFSET_INVALID ) && ( format != EIndexDataFormat::Undefined );
		}

		TS3_FUNC_NO_DISCARD bool isEmpty() const
		{
			return !bufferObject;
		}

		TS3_FUNC_NO_DISCARD bool isValid() const
		{
			return isActive() || isEmpty();
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

		explicit operator bool() const
		{
			return isValid();
		}

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

		TS3_FUNC_NO_DISCARD bool isEmpty() const
		{
			return !bufferObject;
		}

		TS3_FUNC_NO_DISCARD bool isValid() const
		{
			return isActive() || isEmpty();
		}
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
