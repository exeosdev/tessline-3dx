
#include "GLES3_gpuDevice.h"
#include "GLES3_gpuDriver.h"
#include "GLES3_gpuCmdCore.h"
#include <ts3/gpuapi/cmds/commandSystem.h>

namespace ts3::gpuapi
{

	GLES3GPUDevice::GLES3GPUDevice( GLES3GPUDriver * pDriver )
	: GLGPUDevice( pDriver )
	{}

	GLES3GPUDevice::~GLES3GPUDevice() = default;

}
