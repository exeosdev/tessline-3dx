
#include "commonGraphicsConfigImmutableStates.h"

namespace ts3::gpuapi
{

	BlendImmutableState::BlendImmutableState( GPUDevice & pGPUDevice, Bitmask<EBlendConfigFlags> pBlendFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mBlendFlags( pBlendFlags )
	{}

	BlendImmutableState::~BlendImmutableState() = default;


	DepthStencilImmutableState::DepthStencilImmutableState( GPUDevice & pGPUDevice, Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mDepthStencilFlags( pDepthStencilFlags )
	{}

	DepthStencilImmutableState::~DepthStencilImmutableState() = default;


	RasterizerImmutableState::RasterizerImmutableState( GPUDevice & pGPUDevice, Bitmask<ERasterizerConfigFlags> pRasterizerFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mRasterizerFlags( pRasterizerFlags )
	{}

	RasterizerImmutableState::~RasterizerImmutableState() = default;

} // namespace ts3::gpuapi
