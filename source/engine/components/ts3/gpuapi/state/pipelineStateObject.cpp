
#include "pipelineStateObject.h"
#include <ts3/gpuapi/state/renderTargetCommon.h>

namespace ts3::gpuapi
{

	PipelineStateObject::PipelineStateObject( GPUDevice & pGPUDevice )
	: GPUStateObject( pGPUDevice )
	{}

	PipelineStateObject::~PipelineStateObject() = default;


	ComputePipelineStateObject::ComputePipelineStateObject( GPUDevice & pGPUDevice )
	: PipelineStateObject( pGPUDevice )
	{}

	ComputePipelineStateObject::~ComputePipelineStateObject() = default;


	GraphicsPipelineStateObject::GraphicsPipelineStateObject( GPUDevice & pGPUDevice,
	                                                          RenderTargetLayout pRenderTargetLayout,
	                                                          ShaderInputSignature pShaderInputSignature )
	: PipelineStateObject( pGPUDevice )
	, mRenderTargetLayout( std::move( pRenderTargetLayout ) )
	, mShaderInputSignature( std::move( pShaderInputSignature ) )
	{}

	GraphicsPipelineStateObject::~GraphicsPipelineStateObject() = default;

	bool GraphicsPipelineStateObject::createCommonPSOState( const GraphicsPipelineStateObjectCreateInfo & pCreateInfo,
	                                                        CommonPSOState & pOutputState )
	{
		if( !pCreateInfo.shaderBindingDesc || !pCreateInfo.shaderInputSignatureDesc )
		{
			return false;
		}

		if( pCreateInfo.renderTargetLayout )
		{
			pOutputState.renderTargetLayout = *( pCreateInfo.renderTargetLayout );
		}
		else
		{
			if( !createRenderTargetLayout( pCreateInfo.renderTargetLayoutDesc, pOutputState.renderTargetLayout ) )
			{
				return false;
			}
		}

		auto graphicsShaderBinding = createGraphicsShaderBinding( *( pCreateInfo.shaderBindingDesc ) );
		if( !graphicsShaderBinding )
		{
			return false;
		}

		auto shaderInputSignature = createShaderInputSignature( *( pCreateInfo.shaderInputSignatureDesc ) );
		if( !shaderInputSignature )
		{
			// WARN here, no shader resources specified - valid situation, but very unlikely
		}

		pOutputState.shaderBinding = std::move( graphicsShaderBinding );
		pOutputState.shaderInputSignature = std::move( shaderInputSignature );

		return true;
	}


	SeparableGraphicsPipelineStateObject::SeparableGraphicsPipelineStateObject( GPUDevice & pGPUDevice,
	                                                                            RenderTargetLayout pRenderTargetLayout,
	                                                                            GraphicsShaderBinding pShaderBinding,
	                                                                            ShaderInputSignature pShaderInputSignature,
	                                                                            const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet )
	: GraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderInputSignature ) )
	, mShaderBinding( std::move( pShaderBinding ) )
	, mSeparableDescriptorSet( pSeparableDescriptorSet )
	{}

	SeparableGraphicsPipelineStateObject::~SeparableGraphicsPipelineStateObject() = default;

} // namespace ts3::gpuapi
