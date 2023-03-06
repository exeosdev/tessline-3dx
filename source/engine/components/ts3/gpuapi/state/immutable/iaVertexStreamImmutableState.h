
#pragma once

#ifndef __TS3_GPUAPI_IA_VERTEX_STREAM_IMMUTABLE_STATE_H__
#define __TS3_GPUAPI_IA_VERTEX_STREAM_IMMUTABLE_STATE_H__

#include "../graphicsPipelineImmutableState.h"
#include "../inputAssemblerCommon.h"

namespace ts3::gpuapi
{

	struct IAVertexStreamStateCommonProperties
	{
	};

	/// @brief
	class IAVertexStreamImmutableState : public GraphicsPipelineImmutableState
	{
	public:
		IAVertexStreamStateCommonProperties const mCommonProperties;

	public:
		IAVertexStreamImmutableState( const IAVertexStreamImmutableState & ) = delete;
		IAVertexStreamImmutableState & operator=( const IAVertexStreamImmutableState & ) = delete;

		IAVertexStreamImmutableState( GPUDevice & pGPUDevice );
		virtual ~IAVertexStreamImmutableState() = default;

		TS3_ATTR_NO_DISCARD virtual EGraphicsPipelineImmutableStateType queryStateType() const noexcept override final;
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_IA_VERTEX_STREAM_IMMUTABLE_STATE_H__
