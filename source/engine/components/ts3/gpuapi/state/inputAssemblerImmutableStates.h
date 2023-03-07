
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_IMMUTABLE_STATES_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_IMMUTABLE_STATES_H__

#include "graphicsPipelineImmutableState.h"
#include "inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	struct IAInputLayoutStateCommonProperties
	{
		/// Active attributes mask. It contains all bits corresponding to attributes active as part of this descriptor.
		/// @see EIAVertexAttributeFlags
		Bitmask<EIAVertexAttributeFlags> activeAttributesMask;

		/// Number of active attributes enabled for the IA stage.
		uint32 activeAttributesNum;

		/// Primitive topology used to render vertices.
		EPrimitiveTopology primitiveTopology;
	};

	struct IAVertexStreamStateCommonProperties
	{
	};

	/// @brief
	class IAInputLayoutImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		IAInputLayoutStateCommonProperties const mCommonProperties;

	public:
		IAInputLayoutImmutableState( const IAInputLayoutImmutableState & ) = delete;
		IAInputLayoutImmutableState & operator=( const IAInputLayoutImmutableState & ) = delete;

		IAInputLayoutImmutableState( GPUDevice & pGPUDevice, const IAInputLayoutStateCommonProperties & pCommonProperties );
		virtual ~IAInputLayoutImmutableState();

		TS3_ATTR_NO_DISCARD virtual EGraphicsPipelineImmutableStateType queryStateType() const noexcept override final;
	};

	/// @brief
	class IAVertexStreamImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		IAVertexStreamStateCommonProperties const mCommonProperties;

	public:
		IAVertexStreamImmutableState( const IAVertexStreamImmutableState & ) = delete;
		IAVertexStreamImmutableState & operator=( const IAVertexStreamImmutableState & ) = delete;

		IAVertexStreamImmutableState( GPUDevice & pGPUDevice, const IAVertexStreamStateCommonProperties & pCommonProperties );
		virtual ~IAVertexStreamImmutableState();

		TS3_ATTR_NO_DISCARD virtual EGraphicsPipelineImmutableStateType queryStateType() const noexcept override final;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_IMMUTABLE_STATES_H__
