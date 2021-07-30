
#include <ts3/gpuapiGL/GL_gpuDriver.h>
#include <ts3/gpuapiGL/GL_gpuDevice.h>
#include <ts3/gpuapiGL/GL_presentationLayer.h>
#include <ts3/system/displayManager.h>

namespace ts3::gpuapi
{

	GLGPUDriver::GLGPUDriver( SysGLCoreDeviceHandle pSysGLCoreDevice )
	: GPUDriver( pSysGLCoreDevice->mSysContext )
	, mSysGLCoreDevice( std::move( pSysGLCoreDevice ) )
	{ }

	GLGPUDriver::~GLGPUDriver() = default;

	SysGLCoreDeviceHandle GLGPUDriver::initializeSysGLCoreDevice( SysContextHandle pSysContext )
	{
		try
		{
			auto sysDisplayManager = SysDisplayManager::create( pSysContext );
			auto sysGLCoreDevice = SysGLCoreDevice::create( sysDisplayManager );

			sysGLCoreDevice->initializePlatform();

			return sysGLCoreDevice;
		}
		catch ( ... )
		{}

		return nullptr;
	}

}
