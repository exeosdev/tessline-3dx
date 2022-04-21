
#include "DX11_pipelineStateObject.h"
#include "../DX11_gpuDevice.h"
#include "../resources/DX11_shader.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3
{
namespace gpuapi
{

	DX11GraphicsPipelineStateObject::DX11GraphicsPipelineStateObject( DX11GPUDevice & pGPUDevice,
	                                                                  RenderTargetLayout pRenderTargetLayout,
	                                                                  GraphicsShaderBinding pShaderBinding,
	                                                                  ShaderInputSignature pShaderInputSignature,
	                                                                  const GraphicsPipelineStateDescriptorSet & pSeparableDescriptorSet )
	: SeparableGraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderBinding ), std::move( pShaderInputSignature ), pSeparableDescriptorSet )
	{}

	DX11GraphicsPipelineStateObject::~DX11GraphicsPipelineStateObject() = default;

	GpaHandle<DX11GraphicsPipelineStateObject> DX11GraphicsPipelineStateObject::create( DX11GPUDevice & pGPUDevice,
	                                                                                    const GraphicsPipelineStateObjectCreateInfo & pCreateInfo )
	{
		CommonPSOState commonPSOState;
		if( !createCommonPSOState( pCreateInfo, commonPSOState ) )
		{
			return nullptr;
		}

		auto * vertexShader = commonPSOState.shaderBinding[EShaderType::VertexShader];
		if( !vertexShader )
		{
			return nullptr;
		}

		GraphicsPipelineStateDescriptorSet graphicsDescriptorSet;
		graphicsDescriptorSet.blendDescriptorID = pGPUDevice.createBlendDescriptor( pCreateInfo.blendDesc );
		graphicsDescriptorSet.depthStencilDescriptorID = pGPUDevice.createDepthStencilDescriptor( pCreateInfo.depthStencilDesc );
		graphicsDescriptorSet.rasterizerDescriptorID = pGPUDevice.createRasterizerDescriptor( pCreateInfo.rasterizerDesc );
		graphicsDescriptorSet.vertexInputFormatDescriptorID = pGPUDevice.createVertexInputFormatDescriptor( pCreateInfo.vertexInputFormatDesc, *vertexShader );

		auto pipelineStateObject = createDynamicInterfaceObject<DX11GraphicsPipelineStateObject>( pGPUDevice,
		                                                                                          std::move( commonPSOState.renderTargetLayout ),
		                                                                                          std::move( commonPSOState.shaderBinding ),
		                                                                                          std::move( commonPSOState.shaderInputSignature ),
		                                                                                          graphicsDescriptorSet );

		return pipelineStateObject;
	}

} /* namespace ts3 */
} /* namespace gpuapi */
