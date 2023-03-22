
#include "commonGraphicsConfigImmutableStates.h"

namespace ts3::gpuapi
{

	BlendImmutableState::BlendImmutableState( GPUDevice & pGPUDevice, Bitmask<EBlendConfigFlags> pBlendFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mBlendFlags( pBlendFlags & E_BLEND_CONFIG_MASK_ALL )
	{}

	BlendImmutableState::~BlendImmutableState() = default;


	DepthStencilImmutableState::DepthStencilImmutableState( GPUDevice & pGPUDevice, Bitmask<EDepthStencilConfigFlags> pDepthStencilFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mDepthStencilFlags( pDepthStencilFlags & E_DEPTH_STENCIL_CONFIG_MASK_ALL )
	{}

	DepthStencilImmutableState::~DepthStencilImmutableState() = default;


	RasterizerImmutableState::RasterizerImmutableState( GPUDevice & pGPUDevice, Bitmask<ERasterizerConfigFlags> pRasterizerFlags )
	: GraphicsPipelineImmutableState( pGPUDevice )
	, mRasterizerFlags( pRasterizerFlags & E_RASTERIZER_CONFIG_MASK_ALL )
	{}

	RasterizerImmutableState::~RasterizerImmutableState() = default;

} // namespace ts3::gpuapi
