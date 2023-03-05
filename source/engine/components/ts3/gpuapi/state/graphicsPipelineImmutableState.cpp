
#include "graphicsPipelineImmutableState.h"

namespace ts3::gpuapi
{

	GraphicsPipelineImmutableState::GraphicsPipelineImmutableState( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	GraphicsPipelineImmutableState::~GraphicsPipelineImmutableState() = default;

	EGraphicsPipelineImmutableStateType GraphicsPipelineImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::Unknown;
	}

} // namespace ts3::gpuapi
