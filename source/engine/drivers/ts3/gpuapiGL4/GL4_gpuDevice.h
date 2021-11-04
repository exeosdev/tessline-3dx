
#pragma once

#ifndef __TS3DEVICE_GRAPHICS_GL4_GRAPHICS_DEVICE_H__
#define __TS3DEVICE_GRAPHICS_GL4_GRAPHICS_DEVICE_H__

#include "GL4_prerequisites.h"
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/state/GL_pipelineStateObject.h>

namespace ts3
{
namespace gpuapi
{

	struct GL4GPUDeviceCreateInfo : public GPUDeviceCreateInfo
	{
	};

	class TS3GX_GL4_CLASS GL4GPUDevice final : public GLGPUDevice
	{
	public:
		explicit GL4GPUDevice( GL4GPUDriver & pDriver );
		virtual ~GL4GPUDevice();

		static GL4GPUDeviceHandle create( GL4GPUDriver & pDriver, const GL4GPUDeviceCreateInfo & pCreateInfo );
	};

} /* namespace ts3 */
} /* namespace gpuapi */

#endif // __TS3DEVICE_GRAPHICS_GL4_GRAPHICS_DEVICE_H__
