
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__

#include "inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	/// @brief A definition of a vertex attribute. Associates IAVertexInputAttributeInfo with a specific attribute index.
	struct IAVertexInputAttributeDesc
	{
		/// Attribute index this definition refers to.
		EIAVertexAttributeIndex attributeIndex;

		/// Attribute definition.
		IAVertexInputAttributeInfo attributeInfo;
	};

	/// @brief
	struct IAVertexInputAttributeArray
	{
		using AttributeInfoArray = std::array<IAVertexInputAttributeInfo, E_GPU_SYSTEM_METRIC_IA_MAX_VERTEX_ATTRIBUTES_NUM>;

		AttributeInfoArray attributes;
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask{ 0 };
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

	struct IAVertexStreamDescriptorCreateInfo
	{
		const IAVertexInputAttributeArray * inputAttributeArray = nullptr;
		ArrayView<IAVertexInputAttributeDesc> inputAttributeDefinitions;
	};

	class IAVertexFormatDescriptor
	{
	public:
		/// Active attributes mask. It contains all bits corresponding to attributes active as part of this descriptor.
		/// @see EIAVertexAttributeFlags
		const Bitmask<EIAVertexAttributeFlags> mActiveAttributesMask;

		/// Number of active attributes enabled for the IA stage.
		const uint32 mActiveAttributesNum;

	public:
		IAVertexFormatDescriptor( Bitmask<EIAVertexAttributeFlags> pActiveAttributesMask, uint32 pActiveAttributesNum );
		virtual ~IAVertexFormatDescriptor();

		TS3_FUNC_NO_DISCARD bool isAttributeSet( EIAVertexAttributeIndex pAttributeIndex ) const noexcept
		{
			return mActiveAttributesMask.isSet( ecMakeIAVertexAttributeFlag( pAttributeIndex ) );
		}

		TS3_FUNC_NO_DISCARD bool isValid() const
		{
			return ( mActiveAttributesNum > 0 ) && ( mActiveAttributesNum <= E_IA_CONSTANT_MAX_VERTEX_ATTRIBUTES_NUM );
		}
	};

	TS3_FUNC_NO_DISCARD IAVertexInputAttributeArray createIAVertexInputAttributeArray( const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions );

}; // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
