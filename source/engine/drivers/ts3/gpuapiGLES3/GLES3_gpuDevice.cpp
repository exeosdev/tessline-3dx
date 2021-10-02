
#include "GLES3_gpuDevice.h"
#include "GLES3_gpuDriver.h"
#include "GLES3_gpuCmdCore.h"
#include <ts3/gpuapiGL/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GLES3GPUDevice::GLES3GPUDevice( GLES3GPUDriver & pDriver )
	: GLGPUDevice( pDriver )
	{}

	GLES3GPUDevice::~GLES3GPUDevice() = default;

	GLES3GPUDeviceHandle GLES3GPUDevice::create( GLES3GPUDriver & pDriver, const GLES3GPUDeviceCreateInfo & pCreateInfo )
	{
		auto driverConfigFlags = pDriver.getConfigFlags();
		auto gles3GPUDevice = createGPUAPIObject<GLES3GPUDevice>( pDriver );

		gles3GPUDevice->initializeCommandSystem();

		return gles3GPUDevice;
	}

}
