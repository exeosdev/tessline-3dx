
#pragma once

#ifndef __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__
#define __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__

#include "commonGPUStateDefs.h"

namespace ts3::gpuapi
{

	class GraphicsPipelineDescriptor : public GPUBaseObject
	{
	public:
		GraphicsPipelineDescriptor();
		virtual ~GraphicsPipelineDescriptor();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_GRAPHICS_PIPELINE_DESCRIPTOR_H__
