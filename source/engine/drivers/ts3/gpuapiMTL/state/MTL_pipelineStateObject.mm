
#include "MTL_pipelineStateObject.h"
#include "../MTL_gpuDevice.h"

#include <Metal/MTLRenderPipeline.h>

namespace ts3::gpuapi
{

	MetalGraphicsPipelineStateObject::MetalGraphicsPipelineStateObject(
			MetalGPUDevice & pGPUDevice,
			RenderTargetLayout pRenderTargetLayout,
			ShaderInputSignature pShaderInputSignature,
			id<MTLRenderPipelineState> pMTLPipelineState )
	: GraphicsPipelineStateObject(
		pGPUDevice,
		std::move( pRenderTargetLayout ),
		std::move( pShaderInputSignature ) )
	, mMTLPipelineState( pMTLPipelineState )
	{}

	MetalGraphicsPipelineStateObject::~MetalGraphicsPipelineStateObject() = default;

	GpaHandle<MetalGraphicsPipelineStateObject> MetalGraphicsPipelineStateObject::create(
			MetalGPUDevice & pGPUDevice,
			const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
	@autoreleasepool
	{
		auto & mtlDevice = pGPUDevice.mMTLDevice;

		auto * pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	}
	}

}
