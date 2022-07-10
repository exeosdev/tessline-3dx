
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__

#include "inputAssemblerCommon.h"
#include "graphicsPipelineDescriptor.h"

namespace ts3::gpuapi
{

	/// @brief
	struct IAVertexStreamBufferBinding
	{
		using VertexBufferBindingArray = std::array<IAVertexBufferBinding, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_STREAMS_NUM>;
		using VertexBufferIndexArray = std::vector<input_assembler_index_t>;
		using VertexBufferRangeArray = std::vector<IAVertexBufferRange>;

		Bitmask<EIAVertexStreamBindingFlags> activeBindingsMask{ 0 };
		uint32 activeVertexBufferBindingsNum{ 0 };
		uint32 activeVertexBufferRangesNum{ 0 };
		IAIndexBufferBinding indexBufferBinding;
		VertexBufferBindingArray vertexBufferBindingArray;
		VertexBufferIndexArray vertexBufferActiveIndices;
		VertexBufferRangeArray vertexBufferActiveRanges;

		TS3_FUNC_NO_DISCARD IAVertexBufferBinding & operator[]( input_assembler_index_t pVBIndex ) noexcept
		{
			ts3DebugAssert( ecIsIAVertexStreamVBIndexValid( pVBIndex ) );
			return vertexBufferBindingArray[pVBIndex];
		}

		TS3_FUNC_NO_DISCARD const IAVertexBufferBinding & operator[]( input_assembler_index_t pVBIndex ) const noexcept
		{
			ts3DebugAssert( ecIsIAVertexStreamVBIndexValid( pVBIndex ) );
			return vertexBufferBindingArray[pVBIndex];
		}

		TS3_FUNC_NO_DISCARD bool isEmpty() const noexcept
		{
			return activeVertexBufferBindingsNum == 0;
		}

		TS3_FUNC_NO_DISCARD explicit operator bool() const noexcept
		{
			return !isEmpty();
		}
	};

	/// @brief A definition of a vertex buffer binding. Associates IAVertexBufferBinding with a specific VB binding point.
	struct IAVertexBufferBindingDesc
	{
		/// VB binding point index this binding refers to.
		input_assembler_index_t streamIndex{ E_IA_CONSTANT_VERTEX_STREAM_INDEX_UNDEFINED };

		/// Binding definition.
		IAVertexBufferBinding vbBinding;
	};

	/// @brief Create info struct for IAVertexStreamDescriptor.
	struct IAVertexStreamDescriptorCreateInfo
	{
		/// A pointer to a buffer binding state. If it is not null, the driver will use it for descriptor creation.
		/// If the specified  object is not valid, descriptor creation fails immediately.
		/// @see IAVertexStreamBufferBinding
		/// @see IAVertexFormatDescriptor::validateInputAttributeArray
		const IAVertexStreamBufferBinding * bufferBindingState = nullptr;

		/// An array with definitions of vertex buffer bindings. It is used when bufferBindingState is null.
		ArrayView<IAVertexBufferBindingDesc> vertexBufferBindings;

		/// A definition of the index buffer binding. It is used when bufferBindingState is null.
		IAIndexBufferBinding indexBufferBinding;
	};

	/// @brief
	class IAVertexStreamDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		/// Binding mask. It contains all bits corresponding to bindings active as part of this descriptor.
		/// @see EIAVertexStreamBindingFlags
		const Bitmask<EIAVertexStreamBindingFlags> mActiveBindingsMask;

		/// Number of active vertex buffers bound to the IA stage.
		const uint16 mActiveVertexBufferBindingsNum;

		/// Number of active vertex buffer ranges.
		const uint16 mActiveVertexBufferRangesNum;

	public:
		IAVertexStreamDescriptor(
				GPUDevice & pGPUDevice,
				pipeline_descriptor_id_t pDescriptorID,
				const IAVertexStreamBufferBinding & pBufferBinding );

		virtual ~IAVertexStreamDescriptor();

		/// @brief
		TS3_FUNC_NO_DISCARD bool isIndexBufferSet() const noexcept
		{
			return mActiveBindingsMask.isSet( E_IA_VERTEX_STREAM_BINDING_FLAG_INDEX_BUFFER );
		}

		/// @brief
		TS3_FUNC_NO_DISCARD bool isVertexBufferSet( input_assembler_index_t pVBIndex ) const noexcept
		{
			return mActiveBindingsMask.isSet( ecMakeIAVertexStreamVBFlag( pVBIndex ) );
		}
	};

	// State Management Utils
	namespace smu
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexStreamBufferBinding createIAVertexStreamBufferBinding(
				const ArrayView<IAVertexBufferBindingDesc> & pVertexBufferBindings,
				const IAIndexBufferBinding & pIndexBufferBinding );

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateIAVertexStreamBufferBinding(
				const IAVertexStreamBufferBinding & pBufferBinding ) noexcept;

	}

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_STREAM_H__
