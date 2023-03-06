
#include "inputAssemblerImmutableStates.h"

namespace ts3::gpuapi
{

	IAInputLayoutImmutableState::IAInputLayoutImmutableState( GPUDevice & pGPUDevice )
	: GraphicsPipelineImmutableState( pGPUDevice )
	{}

	IAInputLayoutImmutableState::~IAInputLayoutImmutableState() = default;

	EGraphicsPipelineImmutableStateType IAInputLayoutImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::IAInputLayout;
	}

}
