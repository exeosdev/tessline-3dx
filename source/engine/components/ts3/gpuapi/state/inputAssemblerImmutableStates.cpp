
#include "inputAssemblerImmutableStates.h"

namespace ts3::gpuapi
{

	IAInputLayoutImmutableState::IAInputLayoutImmutableState(
			GPUDevice & pGPUDevice,
			const IAInputLayoutStateCommonProperties & pCommonProperties )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mCommonProperties( pCommonProperties )
	{}

	IAInputLayoutImmutableState::~IAInputLayoutImmutableState() = default;

	EGraphicsPipelineImmutableStateType IAInputLayoutImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::IAInputLayout;
	}


	IAVertexStreamImmutableState::IAVertexStreamImmutableState(
			GPUDevice & pGPUDevice,
			const IAVertexStreamStateCommonProperties & pCommonProperties )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mCommonProperties( pCommonProperties )
	{}

	IAVertexStreamImmutableState::~IAVertexStreamImmutableState() = default;

	EGraphicsPipelineImmutableStateType IAVertexStreamImmutableState::queryStateType() const noexcept
	{
		return EGraphicsPipelineImmutableStateType::IAVertexStream;
	}

}
