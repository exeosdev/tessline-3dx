
#pragma once

#ifndef __TS3_GPUAPI_IA_INPUT_LAYOUT_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_IA_INPUT_LAYOUT_IMMUTABLE_STATE_H__

#include "../graphicsPipelineImmutableState.h"
#include "../inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	struct IAInputLayoutStateCommonProperties
	{
		/// Active attributes mask. It contains all bits corresponding to attributes active as part of this descriptor.
		/// @see EIAVertexAttributeFlags
		const Bitmask<EIAVertexAttributeFlags> mActiveAttributesMask;

		/// Number of active attributes enabled for the IA stage.
		const uint32 mActiveAttributesNum;

		/// Primitive topology used to render vertices.
		const EPrimitiveTopology mPrimitiveTopology;
	};

	/// @brief
	class IAInputLayoutImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		IAInputLayoutStateCommonProperties const mCommonProperties;

	public:
		IAInputLayoutImmutableState( const IAInputLayoutImmutableState & ) = delete;
		IAInputLayoutImmutableState & operator=( const IAInputLayoutImmutableState & ) = delete;

		IAInputLayoutImmutableState( GPUDevice & pGPUDevice );
		virtual ~IAInputLayoutImmutableState() = default;

		TS3_ATTR_NO_DISCARD virtual EGraphicsPipelineImmutableStateType queryStateType() const noexcept override final;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_IA_INPUT_LAYOUT_IMMUTABLE_STATE_H__
