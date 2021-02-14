
#include "GLES3_gpuDriver.h"
#include "GLES3_gpuDevice.h"
#include <ts3/gpuapiGL/system/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GLES3GPUDriver::GLES3GPUDriver( ts3::SysGfxGLSubsystem * pExfGLSubsystem )
	: GLGPUDriver( pExfGLSubsystem )
	{ }

	GLES3GPUDriver::~GLES3GPUDriver()
	{ }

	GLES3GPUDriverHandle GLES3GPUDriver::create( const GLES3GPUDriverCreateInfo & pCreateInfo )
	{
		auto * exfGLSubsystem = initializeExfGLSubsystem( pCreateInfo.exfSystemContext );
		if( exfGLSubsystem == nullptr )
		{
			return nullptr;
		}
		return createGPUAPIObject<GLES3GPUDriver>( exfGLSubsystem );
	}

	GPUDeviceHandle GLES3GPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		GLES3GPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;
		return GLES3GPUDevice::create( this, createInfo );
	}

}
