
#include "pipelineStateObject.h"
#include <ts3/gpuapi/state/renderTargetCommon.h>

namespace ts3::gpuapi
{

	PipelineStateObject::PipelineStateObject( GPUDevice & pGPUDevice )
	: GPUDeviceChildObject( pGPUDevice )
	{}

	PipelineStateObject::~PipelineStateObject() = default;


	ComputePipelineStateObject::ComputePipelineStateObject( GPUDevice & pGPUDevice )
	: PipelineStateObject( pGPUDevice )
	{}

	ComputePipelineStateObject::~ComputePipelineStateObject() = default;


	GraphicsPipelineStateObject::GraphicsPipelineStateObject(
			GPUDevice & pGPUDevice,
			RenderTargetLayout pRenderTargetLayout,
			ShaderInputSignature pShaderInputSignature )
	: PipelineStateObject( pGPUDevice )
	, mRenderTargetLayout( std::move( pRenderTargetLayout ) )
	, mShaderInputSignature( std::move( pShaderInputSignature ) )
	{}

	GraphicsPipelineStateObject::~GraphicsPipelineStateObject() = default;

} // namespace ts3::gpuapi
