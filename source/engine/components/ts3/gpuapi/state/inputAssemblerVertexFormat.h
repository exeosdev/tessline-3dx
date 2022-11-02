
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__

#include "graphicsPipelineConfigDefs.h"
#include "inputAssemblerCommon.h"
#include "graphicsPipelineDescriptor.h"

namespace ts3::GpuAPI
{

	/// @brief
	class VertexInputLayout
	{
	public:
		/// Active attributes mask. It contains all bits corresponding to attributes active as part of this descriptor.
		/// @see EIAVertexAttributeFlags
		const Bitmask<EIAVertexAttributeFlags> mActiveAttributesMask;

		/// Number of active attributes enabled for the IA stage.
		const uint32 mActiveAttributesNum;

		/// Primitive topology used to render vertices.
		const EPrimitiveTopology mPrimitiveTopology;
	};

	// State Management Utility API
	namespace StateMgmt
	{

		/// @brief Returns
		TS3_GPUAPI_API_NO_DISCARD Bitmask<EIAVertexAttributeFlags> getIAVertexInputActiveAttributesMask(
				const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD uint32 getIAVertexInputActiveAttributesNum(
				const IAVertexAttributeInfoArray & pVertexAttributes ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD IAVertexInputFormat createIAVertexInputFormat(
				const ArrayView<IAVertexInputAttributeDesc> & pAttributeDefinitions ) noexcept;

		/// @brief
		TS3_GPUAPI_API_NO_DISCARD bool validateIAVertexInputFormat( const IAVertexInputFormat & pInputFormat ) noexcept;

	}

}; // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_VERTEX_FORMAT_H__
