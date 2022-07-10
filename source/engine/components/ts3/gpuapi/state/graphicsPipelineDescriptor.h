
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	class GraphicsPipelineDescriptor : public GPUBaseObject
	{
	public:
		pipeline_descriptor_id_t const mDescriptorID;

		EGraphicsPipelineDescriptorType const mDescriptorType;

	public:
		explicit GraphicsPipelineDescriptor( GPUDevice & pGPUDevice,
											 pipeline_descriptor_id_t pDescriptorID,
											 EGraphicsPipelineDescriptorType pDescriptorType );

		virtual ~GraphicsPipelineDescriptor();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__
