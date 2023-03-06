
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
		constexpr auto IA_MAX_VERTEX_ATTRIBUTES_NUM = static_cast<input_assembler_index_t>( cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_MAX_VERTEX_BUFFER_BINDINGS_NUM = static_cast<input_assembler_index_t>( cxdefs::GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_INPUT_STREAM_BINDING_MASK_ALL = IA_VERTEX_BUFFER_BINDING_MASK_ALL | 0x10000u;

		/// @brief
		inline constexpr bool isIAVertexAttributeIndexValid( input_assembler_index_t pIndex )
		{
			return pIndex < IA_MAX_VERTEX_ATTRIBUTES_NUM;
		}

		/// @brief
		inline constexpr uint32 makeIAVertexAttributeFlag( input_assembler_index_t pAttribIndex )
		{
			return ( pAttribIndex < IA_MAX_VERTEX_ATTRIBUTES_NUM ) ? ( 1 << pAttribIndex ) : 0u;
		}

		/// @brief
		inline constexpr bool isIAVertexBufferIndexValid( input_assembler_index_t pStreamIndex )
		{
			return pStreamIndex < IA_MAX_VERTEX_BUFFER_BINDINGS_NUM;
		}

		/// @brief Returns
		inline constexpr uint32 makeIAVertexBufferFlag( input_assembler_index_t pStreamIndex )
		{
			return ( pStreamIndex < IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) ? ( 1 << pStreamIndex ) : 0u;
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
		E_IA_VERTEX_ATTRIBUTE_MASK_ALL_BITS = makeLSFBitmask<uint32>( cxdefs::IA_MAX_VERTEX_ATTRIBUTES_NUM ),
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

		E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT =
				cxdefs::makeIAVertexBufferFlag( cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ),

		E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS =
				makeLSFBitmask<uint32>( cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ),

		E_IA_VERTEX_STREAM_BINDING_MASK_ALL_BITS =
				E_IA_VERTEX_STREAM_BINDING_MASK_VERTEX_BUFFER_ALL_BITS | E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT
	};

	/// @brief Definition of a vertex input attribute.
	struct IAVertexAttributeInfo
	{
		input_assembler_index_t streamIndex{ cxdefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED };

		uint16 instanceRate{ 0 };

		EVertexAttribFormat format{ EVertexAttribFormat::Undefined };

		/// An offset of the attribute from the start of the vertex data.
		/// VERTEX_ATTRIBUTE_OFFSET_APPEND can be specified if the attribute is placed directly after previous one.
		gpu_memory_size_t relativeOffset{ cxdefs::GPU_MEMORY_OFFSET_INVALID };

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
		IAVertexAttributeInfoArray attributeArray;

		Bitmask<EIAVertexAttributeFlags> activeAttributesMask;

		EPrimitiveTopology primitiveTopology;
	};

	/// @brief
	struct IAIndexBufferReference
	{
		GPUBufferReference sourceBuffer;
		gpu_memory_size_t relativeOffset;
		EIndexDataFormat indexFormat;
	};

	/// @brief
	struct IAVertexBufferReference
	{
		GPUBufferReference sourceBuffer;
		gpu_memory_size_t relativeOffset;
		gpu_memory_size_t vertexStride;
	};

	/// @brief
	using IAVertexBufferReferenceArray = std::array<IAVertexBufferReference, cxdefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM>;

	/// @brief
	struct IAVertexStreamDefinition
	{
		IAVertexBufferReferenceArray vertexBufferReferences;

		IAIndexBufferReference indexBufferReference;

		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask;
	};

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
		TS3_GPUAPI_API_NO_DISCARD bool validateInputLayoutDefinition( const IAInputLayoutDefinition & pDefinition ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
