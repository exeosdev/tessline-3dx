
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__

#include "inputAssemblerCommon.h"
#include "graphicsPipelineDescriptor.h"

namespace ts3::gpuapi
{

	/// @brief A definition of a vertex attribute. Associates IAVertexInputAttributeInfo with a specific attribute index.
	struct IAVertexInputAttributeDesc
	{
		/// Attribute index this definition refers to.
		EIAVertexAttributeIndex attributeIndex{ EIAVertexAttributeIndex::Undefined };

		/// Attribute definition.
		IAVertexInputAttributeInfo attributeInfo;
	};

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

		TS3_FUNC_NO_DISCARD IAVertexInputAttributeInfo & operator[]( EIAVertexAttributeIndex pAttributeIndex ) noexcept
		{
			return attributes[static_cast<input_assembler_index_t>( pAttributeIndex )];
		}

		TS3_FUNC_NO_DISCARD const IAVertexInputAttributeInfo & operator[]( EIAVertexAttributeIndex pAttributeIndex ) const noexcept
		{
			return attributes[static_cast<input_assembler_index_t>( pAttributeIndex )];
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
		EPrimitiveTopology primitiveTopology;
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

		///
		const EPrimitiveTopology mPrimitiveTopology;

	public:
		IAVertexFormatDescriptor( GPUDevice & pGPUDevice,
		                          pipeline_descriptor_id_t pDescriptorID,
		                          IAVertexInputAttributeArray pAttributeArray,
		                          EPrimitiveTopology pPrimitiveTopology );

		virtual ~IAVertexFormatDescriptor();

		/// @brief
		TS3_FUNC_NO_DISCARD virtual bool isValid() const noexcept override final;

		TS3_FUNC_NO_DISCARD const IAVertexInputAttributeArray * getSourceAttributeArray() const noexcept;

		/// @brief
		TS3_FUNC_NO_DISCARD bool isAttributeSet( EIAVertexAttributeIndex pAttributeIndex ) const noexcept;

		/// @brief
		TS3_FUNC_NO_DISCARD static bool validateInputAttributeArray( const IAVertexInputAttributeArray & pAttributeArray ) noexcept;

	private:
		std::unique_ptr<IAVertexInputAttributeArray> _sourceAttributeArray;
	};

	TS3_FUNC_NO_DISCARD IAVertexInputAttributeArray createIAVertexInputAttributeArray( const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions );

	TS3_FUNC_NO_DISCARD IAVertexFormatDescriptorHandle createIAVertexFormatDescriptor( const IAVertexFormatDescriptorCreateInfo & pCreateInfo );

}; // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
