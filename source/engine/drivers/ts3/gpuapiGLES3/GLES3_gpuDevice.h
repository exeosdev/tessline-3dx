
#pragma once

#ifndef __TS3DEVICE_GRAPHICS_GLES3_GRAPHICS_DEVICE_H__
#define __TS3DEVICE_GRAPHICS_GLES3_GRAPHICS_DEVICE_H__

#include "GLES3_prerequisites.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/state/GL_pipelineStateObject.h>

namespace ts3::gpuapi
{

	struct GLES3GPUDeviceCreateInfo : public GPUDeviceCreateInfo
	{
	};

	class TS3GX_GLES3_CLASS GLES3GPUDevice final : public GLGPUDevice
	{
	public:
		explicit GLES3GPUDevice( GLES3GPUDriver & pDriver );
		virtual ~GLES3GPUDevice();

		static GLES3GPUDeviceHandle create( GLES3GPUDriver & pDriver, const GLES3GPUDeviceCreateInfo & pCreateInfo );
	};

} // namespace ts3::gpuapi

#endif // __TS3DEVICE_GRAPHICS_GLES3_GRAPHICS_DEVICE_H__
