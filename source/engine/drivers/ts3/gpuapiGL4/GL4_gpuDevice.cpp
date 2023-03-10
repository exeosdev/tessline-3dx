
#include "GL4_gpuDevice.h"
#include "GL4_gpuDriver.h"
#include "GL4_gpuCmdCore.h"
#include <ts3/gpuapiGL/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GL4GPUDevice::GL4GPUDevice( GL4GPUDriver & pDriver )
	: GLGPUDevice( pDriver )
	{}

	GL4GPUDevice::~GL4GPUDevice() = default;

	GL4GPUDeviceHandle GL4GPUDevice::create( GL4GPUDriver & pDriver, const GL4GPUDeviceCreateInfo & pCreateInfo )
	{
		auto driverConfigFlags = pDriver.getConfigFlags();
		auto gl4GPUDevice = createGPUAPIObject<GL4GPUDevice>( pDriver );

		if( driverConfigFlags.isSet( E_GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT ) )
		{
			// gl4GPUDevice->initializeGLDebugOutput();
		}

		return gl4GPUDevice;
	}

} // namespace ts3::gpuapi
