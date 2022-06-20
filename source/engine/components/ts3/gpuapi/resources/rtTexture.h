
#pragma once

#ifndef __TS3_GPUAPI_RT_TEXTURE_H__
#define __TS3_GPUAPI_RT_TEXTURE_H__

#include "gpuResource.h"
#include "textureCommon.h"

namespace ts3::gpuapi
{

	class RTTexture : public GPUResource
	{
	public:
		RTTexture( GPUDevice & pGPUDevice );

		virtual ~RTTexture();
	};

} // namespace ts3::gpuapi

#endif // __TS3_GPUAPI_RT_TEXTURE_H__
