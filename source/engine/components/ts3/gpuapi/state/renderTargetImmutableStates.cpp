
#include "renderTargetImmutableStates.h"

namespace ts3::gpuapi
{

	RenderTargetBindingImmutableState::RenderTargetBindingImmutableState(
			GPUDevice & pGPUDevice,
			const RenderTargetLayout & pRenderTargetLayout )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mRenderTargetLayout( pRenderTargetLayout )
	{}

	RenderTargetBindingImmutableState::~RenderTargetBindingImmutableState() = default;


	RenderPassConfigurationImmutableState::RenderPassConfigurationImmutableState( GPUDevice & pGPUDevice )
	: GraphicsPipelineImmutableState( pGPUDevice )
	{}

	RenderPassConfigurationImmutableState::~RenderPassConfigurationImmutableState() = default;

}
