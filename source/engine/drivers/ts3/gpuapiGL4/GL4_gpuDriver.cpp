
#include "GL4_gpuDriver.h"
#include "GL4_gpuDevice.h"
#include <ts3/gpuapiGL/GL_presentationLayer.h>

namespace ts3::gpuapi
{

	GL4GPUDriver::GL4GPUDriver( SysGLDriverHandle pSysGLDriver )
	: GLGPUDriver( pSysGLDriver )
	{}

	GL4GPUDriver::~GL4GPUDriver() = default;

	GL4GPUDriverHandle GL4GPUDriver::create( const GL4GPUDriverCreateInfo & pCreateInfo )
	{
		constexpr auto gl4SupportedGPUConfigFlags =
			GPU_DRIVER_CONFIG_FLAG_ENABLE_DEBUG_LAYER_BIT |
			GPU_DRIVER_CONFIG_FLAG_ENABLE_SHADER_DEBUG_INFO_BIT;

		auto sysContext = pCreateInfo.sysContext;
		if( !sysContext )
		{
			sysContext = sysCreateContext( pCreateInfo.sysContextCreateInfo );
			if( !sysContext )
			{
				return nullptr;
			}
		}

		auto sysGLDriver = initializeSysGLDriver( sysContext );
		if( !sysGLDriver )
		{
			return nullptr;
		}

		auto gl4Driver = createGPUAPIObject<GL4GPUDriver>( sysGLDriver );
		gl4Driver->setConfigFlags( pCreateInfo.configFlags & gl4SupportedGPUConfigFlags );

		return gl4Driver;
	}

	DisplayManagerHandle GL4GPUDriver::createDefaultDisplayManager()
	{
		return nullptr;
	}

	GPUDeviceHandle GL4GPUDriver::createDevice( const GPUDeviceCreateInfo & pCreateInfo )
	{
		GL4GPUDeviceCreateInfo createInfo;
		createInfo.adapterID = pCreateInfo.adapterID;
		createInfo.flags = pCreateInfo.flags;

		return GL4GPUDevice::create( *this, createInfo );
	}

	EGPUDriverID GL4GPUDriver::queryGPUDriverID() const
	{
		return EGPUDriverID::GDIDGL4;
	}

}
