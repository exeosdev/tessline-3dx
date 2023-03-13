
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"
#include "../resources/gpuBufferReference.h"

namespace ts3::gpuapi
{

	ts3GpaDeclareClassHandle( GPUBuffer );

	/// @brief
	using input_assembler_index_t = uint16;

	namespace cxdefs
	{

		///
		constexpr auto IA_MAX_VERTEX_ATTRIBUTES_NUM = static_cast<input_assembler_index_t>( GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_MAX_VERTEX_BUFFER_BINDINGS_NUM = static_cast<input_assembler_index_t>( GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_INPUT_STREAM_BINDING_MASK_ALL = IA_VERTEX_BUFFER_BINDING_MASK_ALL | 0x10000u;

		/// @brief
		inline constexpr bool isIAVertexAttributeIndexValid( native_uint pIndex )
		{
			return pIndex < IA_MAX_VERTEX_ATTRIBUTES_NUM;
		}

		/// @brief
		inline constexpr uint32 makeIAVertexAttributeFlag( native_uint pAttribIndex )
		{
			return ( pAttribIndex < IA_MAX_VERTEX_ATTRIBUTES_NUM ) ? ( 1 << static_cast<input_assembler_index_t>( pAttribIndex ) ) : 0u;
		}

		/// @brief
		inline constexpr bool isIAVertexBufferIndexValid( native_uint pStreamIndex )
		{
			return pStreamIndex < IA_MAX_VERTEX_BUFFER_BINDINGS_NUM;
		}

		/// @brief Returns
		inline constexpr uint32 makeIAVertexBufferFlag( native_uint pStreamIndex )
		{
			return ( pStreamIndex < IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) ? ( 1 << static_cast<input_assembler_index_t> ( pStreamIndex ) ) : 0u;
		}

		/// @brief Returns
		inline constexpr uint32 makeIAIndexBufferFlag()
		{
			return ( 1 << static_cast<input_assembler_index_t>( IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) );
		}

	}

	/// @brief
	enum EIAVertexAttributeFlags : uint32
	{
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT = cxdefs::makeIAVertexAttributeFlag( 0 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT = cxdefs::makeIAVertexAttributeFlag( 1 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT = cxdefs::makeIAVertexAttributeFlag( 2 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT = cxdefs::makeIAVertexAttributeFlag( 3 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_4_BIT = cxdefs::makeIAVertexAttributeFlag( 4 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_5_BIT = cxdefs::makeIAVertexAttributeFlag( 5 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_6_BIT = cxdefs::makeIAVertexAttributeFlag( 6 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_7_BIT = cxdefs::makeIAVertexAttributeFlag( 7 ),
		E_IA_VERTEX_ATTRIBUTE_MASK_ALL = makeLSFBitmask<uint32>( cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM ),
	};

	/// @brief
	enum EIAVertexStreamBindingFlags : uint32
	{
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_0_BIT = cxdefs::makeIAVertexBufferFlag( 0 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_1_BIT = cxdefs::makeIAVertexBufferFlag( 1 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_2_BIT = cxdefs::makeIAVertexBufferFlag( 2 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_3_BIT = cxdefs::makeIAVertexBufferFlag( 3 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_4_BIT = cxdefs::makeIAVertexBufferFlag( 4 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_5_BIT = cxdefs::makeIAVertexBufferFlag( 5 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_6_BIT = cxdefs::makeIAVertexBufferFlag( 6 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_7_BIT = cxdefs::makeIAVertexBufferFlag( 7 ),

		E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT = cxdefs::makeIAIndexBufferFlag(),

		E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS =
				makeLSFBitmask<uint32>( cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ),

		E_IA_VERTEX_STREAM_BINDING_MASK_ALL =
				E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS | E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT
	};

	/// @brief Definition of a vertex input attribute.
	struct IAVertexAttributeInfo
	{
		input_assembler_index_t streamIndex{ cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED };

		const char * semanticName = ts3::cxdefs::STR_CHAR_EMPTY;

		input_assembler_index_t semanticIndex = 0;

		EVertexAttribFormat format{ EVertexAttribFormat::Undefined };

		/// An offset of the attribute from the start of the vertex data.
		/// VERTEX_ATTRIBUTE_OFFSET_APPEND can be specified if the attribute is placed directly after previous one.
		gpu_memory_size_t relativeOffset{ cxdefs::GPU_MEMORY_OFFSET_INVALID };

		uint16 instanceRate{ 0 };

		/// @brief Returns true if this instance represents a valid vertex attribute.
		TS3_ATTR_NO_DISCARD constexpr bool active() const noexcept
		{
			return cxdefs::isIAVertexBufferIndexValid( streamIndex ) && ( format != EVertexAttribFormat::Undefined );
		}

		/// @brief Returns true if this instance represents a valid vertex attribute.
		TS3_ATTR_NO_DISCARD constexpr bool valid() const noexcept
		{
			return active() && ( relativeOffset != cxdefs::GPU_MEMORY_OFFSET_INVALID );
		}
	};

	/// @brief Typedef for ordered, fixed-size array of vertex attribute definitions.
	using IAVertexAttributeInfoArray = std::array<IAVertexAttributeInfo, cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM>;

	/// @brief
	struct IAInputLayoutDefinition
	{
		EPrimitiveTopology primitiveTopology;
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask;
		IAVertexAttributeInfoArray attributeArray;
	};

	struct IAVertexStreamBufferReference
	{
		GPUBufferReference sourceBuffer;
		gpu_memory_size_t relativeOffset;

		bool empty() const noexcept
		{
			return sourceBuffer.empty();
		}

		gpu_memory_size_t dataOffset() const noexcept
		{
			return sourceBuffer.getRefSubRegion().offset + relativeOffset;
		}

		explicit operator bool() const noexcept
		{
			return !empty();
		}

		void reset()
		{
			sourceBuffer.reset();
			relativeOffset = 0;
		}
	};

	/// @brief
	struct IAIndexBufferReference : public IAVertexStreamBufferReference
	{
		EIndexDataFormat indexFormat;
	};

	/// @brief
	struct IAVertexBufferReference : public IAVertexStreamBufferReference
	{
		gpu_memory_size_t vertexStride;
	};

	/// @brief
	using IAVertexBufferReferenceArray = std::array<IAVertexBufferReference, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM>;

	/// @brief
	struct IAVertexStreamDefinition
	{
		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask;
		IAVertexBufferReferenceArray vertexBufferReferences;
		IAIndexBufferReference indexBufferReference;
	};

	/// @brief Describes a single, continuous range of vertex buffer binding points in the vertex stream.
	/// Used by drivers with support for range-based binding (D3D11, GL4) to reduce the number of API calls.
	struct IAVertexBufferRange
	{
		/// First index of the vertex buffer binding this range defines.
		input_assembler_index_t firstIndex = cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED;

		/// Length of this range, i.e. number of vertex buffers within the range.
		uint16 length = 0;
	};

	using IAVertexBufferBindingIndexList = std::vector<input_assembler_index_t>;

	using IAVertexBufferRangeList = std::vector<IAVertexBufferRange>;

	// State Management Utility API
	namespace smutil
	{

		/// @brief Returns
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EIAVertexAttributeFlags> getIAVertexInputActiveAttributesMask(
				const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD uint32 getIAVertexInputActiveAttributesNum(
				const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexBufferRangeList generateActiveVertexBufferRanges(
				const IAVertexBufferReferenceArray & pVBReferences ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
