
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__

#include "inputAssemblerCommon.h"
#include "graphicsPipelineDescriptor.h"

namespace ts3::gpuapi
{

	/// @brief A common representation of a vertex attribute array (layout/format of all active input attributes).
	/// This is a driver-agnostic structure used by the engine to represent the vertex array part of the IA stage.
	/// It is used to create a VertexStreamDescriptor via IAVertexStreamDescriptorCreateInfo.
	/// @see IAVertexFormatDescriptor
	/// @see IAVertexStreamDescriptorCreateInfo
	struct IAVertexInputAttributeArray
	{
		using AttributeInfoArray = std::array<IAVertexInputAttributeInfo, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;

		/// A fixed array with all vertex attributes. Entry at index N corresponds to IA attribute N.
		AttributeInfoArray attributes;

		/// Bitmask with all bits corresponding to active vertex attributes.
		/// @see EIAVertexAttributeFlags
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask{ 0 };

		///
		uint32 activeAttributesNum{ 0 };

		TS3_FUNC_NO_DISCARD IAVertexInputAttributeInfo & operator[]( input_assembler_index_t pAttributeIndex ) noexcept
		{
			return attributes[pAttributeIndex];
		}

		TS3_FUNC_NO_DISCARD const IAVertexInputAttributeInfo & operator[]( input_assembler_index_t pAttributeIndex ) const noexcept
		{
			return attributes[pAttributeIndex];
		}

		TS3_FUNC_NO_DISCARD bool isEmpty() const noexcept
		{
			return activeAttributesNum == 0;
		}

		TS3_FUNC_NO_DISCARD explicit operator bool() const noexcept
		{
			return !isEmpty();
		}
	};

	/// @brief A definition of a vertex attribute. Associates IAVertexInputAttributeInfo with a specific attribute index.
	struct IAVertexInputAttributeDesc
	{
		/// Attribute index this definition refers to.
		input_assembler_index_t attributeIndex{ E_IA_CONSTANT_VERTEX_ATTRIBUTE_INDEX_UNDEFINED };

		/// Attribute definition.
		IAVertexInputAttributeInfo attributeInfo;
	};

	/// @brief Create info struct for IAVertexFormatDescriptor.
	struct IAVertexFormatDescriptorCreateInfo
	{
		/// A pointer to a vertex attribute array. If it is not null, the driver will use it for descriptor creation.
		/// If the specified  object is not valid, descriptor creation fails immediately.
		/// @see IAVertexInputAttributeArray
		/// @see IAVertexFormatDescriptor::validateInputAttributeArray
		const IAVertexInputAttributeArray * inputAttributeArray = nullptr;

		/// An array with definitions of vertex attributes. It is used when inputAttributeArray is null.
		/// @see IAVertexInputAttributeDesc
		ArrayView<IAVertexInputAttributeDesc> inputAttributeDefinitions;

		///
		/// @see EPrimitiveTopology
		EPrimitiveTopology primitiveTopology = EPrimitiveTopology::Undefined;
	};

	/// @brief
	class IAVertexFormatDescriptor : public GraphicsPipelineDescriptor
	{
	public:
		/// Active attributes mask. It contains all bits corresponding to attributes active as part of this descriptor.
		/// @see EIAVertexAttributeFlags
		const Bitmask<EIAVertexAttributeFlags> mActiveAttributesMask;

		/// Number of active attributes enabled for the IA stage.
		const uint32 mActiveAttributesNum;

		/// Primitive topology used to render vertices.
		const EPrimitiveTopology mPrimitiveTopology;

	public:
		IAVertexFormatDescriptor(
				GPUDevice & pGPUDevice,
				pipeline_descriptor_id_t pDescriptorID,
				const IAVertexInputAttributeArray & pAttributeArray,
				EPrimitiveTopology pPrimitiveTopology );

		virtual ~IAVertexFormatDescriptor();

		/// @brief
		TS3_FUNC_NO_DISCARD bool isAttributeSet( input_assembler_index_t pAttributeIndex ) const noexcept;
	};

	// State Management Utils
	namespace smu
	{

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexInputAttributeArray createIAVertexInputAttributeArray(
				const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateIAVertexInputAttributeArray(
				const IAVertexInputAttributeArray & pAttributeArray ) noexcept;

	}

}; // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
