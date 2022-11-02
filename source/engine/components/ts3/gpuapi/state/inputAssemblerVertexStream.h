
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__

#include "inputAssemblerCommon.h"
#include "graphicsPipelineDescriptor.h"

namespace ts3::GpuAPI
{

	/// @brief
	class VertexStreamBufferBinding
	{
	public:
		/// Binding mask. It contains all bits corresponding to bindings active as part of this descriptor.
		/// @see EIAVertexStreamBindingFlags
		const Bitmask<EIAVertexStreamBindingFlags> mActiveBindingsMask;

		/// Number of active vertex buffers bound to the IA stage.
		const uint32 mActiveVertexBufferBindingsNum;
	};

	// State Management Utilities
	namespace StateMgmt
	{

		/// @brief Describes a single, continuous range of vertex buffer binding points in the vertex stream.
		/// Used by drivers with support for range-based binding (D3D11, GL4) to reduce the number of API calls.
		struct IAVertexBufferRange
		{
			/// First index of the vertex buffer binding this range defines.
			input_assembler_index_t firstIndex{ CxDefs::IA_VERTEX_BUFFER_BINDING_INDEX_UNDEFINED };

			/// Length of this range, i.e. number of vertex buffers within the range.
			input_assembler_index_t length{ 0 };
		};

		/// @brief
		using VertexBufferIndexArray = std::vector<input_assembler_index_t>;

		/// @brief
		using VertexBufferRangeArray = std::vector<IAVertexBufferRange>;

		/// @brief Returns a bitmask representing specified IA buffer bindings state.
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EIAVertexStreamBindingFlags> getIAVertexStreamActiveBindingsMask(
				const IAVertexBufferBindingArray & pVertexBufferBindings,
				const IAIndexBufferBinding & pIndexBufferBinding ) noexcept;

		/// @brief Returns the number of active vertex buffer bindings.
		TS3_GPUAPI_API_NO_DISCARD uint32 getIAVertexBufferActiveBindingsNum(
				const IAVertexBufferBindingArray & pVertexBufferBindings ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexStreamConfiguration createIAVertexStreamConfiguration(
				const ArrayView<IAVertexBufferBindingDesc> & pVertexBufferBindingDefinitions,
				const IAIndexBufferBinding & pIndexBufferBindingDefinition ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD VertexBufferIndexArray generateActiveVertexBufferIndices(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD VertexBufferRangeArray generateActiveVertexBufferRanges(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateIAVertexStreamConfiguration(
				const IAVertexStreamConfiguration & pVertexStreamConfig ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD PipelineDescriptorHash computePipelineDescriptorHash(
				const IAVertexInputLayoutDescriptorCreateInfo & pDescriptorCreateInfo ) noexcept;

	}

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__
