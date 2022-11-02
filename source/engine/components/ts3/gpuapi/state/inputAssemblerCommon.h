
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__

#include "commonGPUStateDefs.h"
#include "../resources/shaderCommon.h"

namespace ts3::GpuAPI
{

	ts3GpaDeclareClassHandle( GPUBuffer );
	ts3GpaDeclareClassHandle( IAVertexInputLayoutDescriptor );
	ts3GpaDeclareClassHandle( IAVertexStreamDescriptor );

	struct IAVertexInputLayoutDescriptorCreateInfo;
	struct IAVertexStreamDescriptorCreateInfo;

	/// @brief
	using input_assembler_index_t = uint16;

	namespace CxDefs
	{

		///
		constexpr auto IA_MAX_VERTEX_ATTRIBUTES_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_ATTRIBUTE_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_ATTRIBUTES_NUM );

		///
		constexpr auto IA_MAX_VERTEX_BUFFER_BINDINGS_NUM = static_cast<input_assembler_index_t>( E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED = Limits<input_assembler_index_t>::maxValue;

		///
		constexpr auto IA_VERTEX_BUFFER_BINDING_MASK_ALL = makeLSFBitmask<input_assembler_index_t>( IA_MAX_VERTEX_BUFFER_BINDINGS_NUM );

		///
		constexpr auto IA_INPUT_STREAM_BINDING_MASK_ALL = IA_VERTEX_BUFFER_BINDING_MASK_ALL | 0x10000u;

		/// @brief
		inline constexpr bool isIAVertexAttributeIndexValid( input_assembler_index_t pIndex )
		{
			return pIndex < CxDefs::IA_MAX_VERTEX_ATTRIBUTES_NUM;
		}

		/// @brief
		inline constexpr uint32 makeIAVertexAttributeFlag( input_assembler_index_t pAttribIndex )
		{
			return ( pAttribIndex < CxDefs::IA_MAX_VERTEX_ATTRIBUTES_NUM ) ? ( 1 << pAttribIndex ) : 0u;
		}

		/// @brief
		inline constexpr bool isIAVertexBufferIndexValid( input_assembler_index_t pStreamIndex )
		{
			return pStreamIndex < CxDefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM;
		}

		/// @brief Returns
		inline constexpr uint32 makeIAVertexBufferFlag( input_assembler_index_t pStreamIndex )
		{
			return ( pStreamIndex < CxDefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM ) ? ( 1 << pStreamIndex ) : 0u;
		}

	}

	/// @brief
	enum EIAVertexAttributeFlags : uint32
	{
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_0_BIT = CxDefs::makeIAVertexAttributeFlag( 0 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_1_BIT = CxDefs::makeIAVertexAttributeFlag( 1 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_2_BIT = CxDefs::makeIAVertexAttributeFlag( 2 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_3_BIT = CxDefs::makeIAVertexAttributeFlag( 3 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_4_BIT = CxDefs::makeIAVertexAttributeFlag( 4 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_5_BIT = CxDefs::makeIAVertexAttributeFlag( 5 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_6_BIT = CxDefs::makeIAVertexAttributeFlag( 6 ),
		E_IA_VERTEX_ATTRIBUTE_FLAG_ATTR_7_BIT = CxDefs::makeIAVertexAttributeFlag( 7 ),
	};

	/// @brief
	enum EIAVertexStreamBindingFlags : uint32
	{
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_0_BIT = CxDefs::makeIAVertexBufferFlag( 0 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_1_BIT = CxDefs::makeIAVertexBufferFlag( 1 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_2_BIT = CxDefs::makeIAVertexBufferFlag( 2 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_3_BIT = CxDefs::makeIAVertexBufferFlag( 3 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_4_BIT = CxDefs::makeIAVertexBufferFlag( 4 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_5_BIT = CxDefs::makeIAVertexBufferFlag( 5 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_6_BIT = CxDefs::makeIAVertexBufferFlag( 6 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_VERTEX_BUFFER_7_BIT = CxDefs::makeIAVertexBufferFlag( 7 ),
		E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER_BIT    = CxDefs::makeIAVertexBufferFlag( CxDefs::IA_MAX_VERTEX_BUFFER_BINDINGS_NUM )
	};

	/// @brief Represents vertex input attribute.
	struct IAVertexAttributeInfo
	{
		input_assembler_index_t streamIndex{ CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED };

		uint16 instanceRate{ 0 };

		EVertexAttribFormat format{ EVertexAttribFormat::Undefined };

		gpu_memory_size_t relativeOffset{ CxDefs::GPU_MEMORY_OFFSET_INVALID };

		/// @brief Returns true if this instance represents a valid vertex attribute.
		TS3_ATTR_NO_DISCARD bool active() const noexcept
		{
			return CxDefs::isIAVertexBufferIndexValid( streamIndex ) && ( format != EVertexAttribFormat::Undefined );
		}
	};

	/// Describes the binding of an index buffer, used for indexed draw calls. IndexBuffer is not required
	/// and the binding may be empty. In such case, any indexed draw call executed on a context will fail.
	struct IAIndexBufferReference
	{
		/// A pointer to an existing buffer containing indices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_INDEX_BUFFER_BIT bit set.
		GPUBufferHandle bufferObject{ nullptr };

		/// A format of the data within the bound buffer's range.
		EIndexDataFormat dataFormat{ EIndexDataFormat::Undefined };

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset{ CxDefs::GPU_MEMORY_OFFSET_INVALID };

		/// @brief Returns true if this instance represents a valid binding.
		TS3_ATTR_NO_DISCARD bool active() const noexcept
		{
			return bufferObject && ( dataFormat != EIndexDataFormat::Undefined ) && ( dataOffset != CxDefs::GPU_MEMORY_OFFSET_INVALID );
		}
	};

	/// @brief Describes the binding of one of the vertex buffers used to source vertices data during rendering.
	struct IAVertexBufferReference
	{
		/// A pointer to an existing buffer containing vertices data.
		/// Buffer must have been created with the E_GPU_BUFFER_BIND_FLAG_VERTEX_BUFFER_BIT bit set.
		GPUBufferHandle bufferObject{ nullptr };

		/// An offset from the beginning of the buffer's storage.
		gpu_memory_size_t dataOffset{ CxDefs::GPU_MEMORY_OFFSET_INVALID };

		/// Size of a single vertex instance within the buffer. Basically, it's a size of a struct
		/// used to represent vertex sub-data stored in this buffer. Cannot be 0.
		gpu_memory_size_t dataStride{ 0 };

		/// @brief Returns true if this instance represents a valid binding.
		TS3_ATTR_NO_DISCARD bool active() const noexcept
		{
			return bufferObject && ( dataOffset != CxDefs::GPU_MEMORY_OFFSET_INVALID ) && ( dataStride > 0 );
		}
	};

	/// @brief Typedef for ordered, fixed-size array of vertex attribute definitions.
	using IAVertexAttributeInfoArray = std::array<IAVertexAttributeInfo, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;

	/// @brief Typedef for ordered, fixed-size array of vertex buffer binding definitions.
	using IAVertexBufferReferenceArray = std::array<IAVertexBufferReference, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_BUFFER_BINDINGS_NUM>;

	/// @brief A common representation of a vertex attribute array (layout/format of all active input attributes).
	/// This is a driver-agnostic structure used by the engine to represent the vertex array part of the IA stage.
	/// It is used to create a IAVertexInputLayoutDescriptor via IAVertexInputLayoutDescriptorCreateInfo.
	/// @note
	/// This structure can be initialised directly, but make sure it passes the validation (CSA::validateIAVertexInputFormat)
	/// before using it for the descriptor creation. To avoid mistakes, CSA::createIAVertexInputFormat can be used to produce
	/// a valid vertex format from a list of attribute definitions.
	struct IAVertexFormatDefinition
	{
		/// A fixed array with all vertex attributes. This array is ordered: entry at index N corresponds to IA attribute N.
		IAVertexAttributeInfoArray attributeArray;
	};

	/// @brief A definition of a vertex attribute. Associates IAVertexAttributeInfo with a specific attribute index.
	/// Used to create IAVertexInputFormat via createIAVertexInputFormat() function.
	struct IAVertexAttributeBindingDesc
	{
		/// Attribute index this definition refers to.
		input_assembler_index_t attributeIndex{ CxDefs::IA_VERTEX_ATTRIBUTE_INDEX_UNDEFINED };

		/// Attribute definition.
		IAVertexAttributeInfo attributeInfo;
	};

	/// @brief
	/// @note
	/// This structure can be initialised directly, but make sure it passes the validation (CSA::validateIAVertexStreamConfiguration)
	/// before using it for the descriptor creation. To avoid mistakes, CSA::createIAVertexStreamConfiguration can be used to produce
	/// a valid vertex stream configuration from a list of IA buffer bindings.
	/// @see IAVertexStreamDescriptor
	/// @see IAVertexStreamDescriptorCreateInfo
	struct IAVertexStreamDefinition
	{
		IAIndexBufferReference indexBufferReference;

		IAVertexBufferReferenceArray vertexBufferReferenceArray;
	};

	/// @brief A definition of a vertex buffer binding. Associates IAVertexBufferBinding with a specific VB binding point.
	struct IAVertexBufferBindingDesc
	{
		/// VB binding point index this binding refers to.
		input_assembler_index_t streamIndex{ CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED };

		/// Binding definition.
		IAVertexBufferReference vertexBufferReference;
	};

	// State Management Utility API
	namespace StateUtils
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexAttributeInfoArray initializeVertexAttributeArray(
				const ArrayView<IAVertexAttributeBindingDesc> & pAttributes ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD pipeline_internal_state_hash_t computePipelineInternalStateHash(
				const IAVertexFormatDefinition & pVertexFormatDefinition ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD pipeline_internal_state_hash_t computePipelineInternalStateHash(
				const IAInputBuffersDefinition & pVertexFormatDefinition ) noexcept;

	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_COMMON_H__
