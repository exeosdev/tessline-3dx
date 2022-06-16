
#include "DX11_pipelineStateObject.h"
#include "../DX11_gpuDevice.h"
#include "../resources/DX11_shader.h"
#include <ts3/gpuapi/resources/shader.h>

namespace ts3::gpuapi
{

	DX11GraphicsPipelineStateObject::DX11GraphicsPipelineStateObject( DX11GPUDevice & pGPUDevice,
	                                                                  RenderTargetLayout pRenderTargetLayout,
	                                                                  GraphicsShaderBinding pShaderBinding,
	                                                                  ShaderInputSignature pShaderInputSignature,
	                                                                  const SeparableGraphicsStateDescriptorSet & pSstateDescriptors )
	: SeparableGraphicsPipelineStateObject( pGPUDevice, std::move( pRenderTargetLayout ), std::move( pShaderBinding ), std::move( pShaderInputSignature ), pSstateDescriptors )
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

		SeparableGraphicsStateDescriptorSet sstateDescriptors;
		sstateDescriptors.blendDescriptorID = pGPUDevice.createBlendDescriptor( pCreateInfo.blendDesc );
		sstateDescriptors.depthStencilDescriptorID = pGPUDevice.createDepthStencilDescriptor( pCreateInfo.depthStencilDesc );
		sstateDescriptors.rasterizerDescriptorID = pGPUDevice.createRasterizerDescriptor( pCreateInfo.rasterizerDesc );
		sstateDescriptors.vertexInputFormatDescriptorID = pGPUDevice.createVertexInputFormatDescriptor( pCreateInfo.vertexInputFormatDesc, *vertexShader );

		auto pipelineStateObject = createDynamicInterfaceObject<DX11GraphicsPipelineStateObject>( pGPUDevice,
		                                                                                          std::move( commonPSOState.renderTargetLayout ),
		                                                                                          std::move( commonPSOState.shaderBinding ),
		                                                                                          std::move( commonPSOState.shaderInputSignature ),
		                                                                                          sstateDescriptors );

		return pipelineStateObject;
	}

} // namespace ts3::gpuapi
