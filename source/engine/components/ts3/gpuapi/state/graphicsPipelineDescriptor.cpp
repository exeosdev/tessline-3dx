
#include "graphicsPipelineDescriptor.h"

namespace ts3::gpuapi
{

	GraphicsPipelineDescriptor::GraphicsPipelineDescriptor( GPUDevice & pGPUDevice,
	                                                        pipeline_descriptor_id_t pDescriptorID,
	                                                        EGraphicsPipelineDescriptorType pDescriptorType )
	: GPUBaseObject( pGPUDevice )
	, mDescriptorID( pDescriptorID )
	, mDescriptorType( pDescriptorType )
	{}

	GraphicsPipelineDescriptor::~GraphicsPipelineDescriptor() = default;

} // namespace ts3::gpuapi
