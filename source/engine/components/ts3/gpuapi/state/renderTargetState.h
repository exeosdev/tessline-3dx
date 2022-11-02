
#pragma once

#ifndef __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__
#define __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__

#include "renderTargetCommon.h"
#include <ts3/gpuapi/resources/renderBuffer.h>

namespace ts3::GpuAPI
{

	struct RTClearValue
	{
		math::RGBAColorR32Norm colorClearValue;
		float depthClearValue;
		uint8 stencilClearValue;
	};

	class RTAttachmentReference
	{
	};

	class RTAttachmentDescriptor
	{
	};

} // namespace ts3::GpuAPI

#endif // __TS3_GPUAPI_INPUT_ASSEMBLER_BINDING_H__
